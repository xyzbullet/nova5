#include "network_stack.h"

#include <stdlib.h>
#include <string.h>

#define NOVA_SOCKET_TABLE_SIZE 64

static struct nova_net_device *g_nova_netdev_registry = NULL;
static struct nova_socket *g_socket_table[NOVA_SOCKET_TABLE_SIZE];
static int g_next_socket_fd = 1;
static int g_network_initialized = 0;

static struct nova_socket *nova_socket_lookup(int sockfd) {
    if (sockfd <= 0 || sockfd >= NOVA_SOCKET_TABLE_SIZE) {
        return NULL;
    }
    return g_socket_table[sockfd];
}

int nova_network_init(void) {
    memset(g_socket_table, 0, sizeof(g_socket_table));
    g_next_socket_fd = 1;
    g_network_initialized = 1;
    g_nova_netdev_registry = NULL;
    return 0;
}

int nova_network_shutdown(void) {
    struct nova_net_device *dev = g_nova_netdev_registry;
    while (dev != NULL) {
        struct nova_net_device *next = dev->next;
        dev->next = NULL;
        dev = next;
    }
    g_nova_netdev_registry = NULL;
    g_network_initialized = 0;
    return 0;
}

int nova_socket(int domain, int type, int protocol) {
    struct nova_socket *sock;
    int fd;

    if (!g_network_initialized) {
        nova_network_init();
    }

    if (domain != NOVA_AF_UNSPEC && domain != NOVA_AF_INET &&
        domain != NOVA_AF_INET6 && domain != NOVA_AF_UNIX) {
        return -1;
    }

    switch (type) {
    case NOVA_SOCK_STREAM:
    case NOVA_SOCK_DGRAM:
    case NOVA_SOCK_RAW:
    case NOVA_SOCK_SEQPACKET:
        break;
    default:
        return -1;
    }

    if (g_next_socket_fd >= NOVA_SOCKET_TABLE_SIZE) {
        return -1;
    }

    fd = g_next_socket_fd++;
    sock = calloc(1, sizeof(*sock));
    if (sock == NULL) {
        return -1;
    }

    sock->family = (enum nova_af)domain;
    sock->type = (enum nova_sock_type)type;
    sock->protocol = protocol;
    sock->state = 0;
    sock->flags = 0;
    sock->ops = NULL;
    sock->private_data = NULL;
    sock->dev = nova_netdev_find("lo");
    g_socket_table[fd] = sock;
    return fd;
}

int nova_bind(int sockfd, const struct nova_sockaddr *addr, socklen_t addrlen) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    (void)addr;
    (void)addrlen;
    if (sock == NULL) {
        return -1;
    }
    return 0;
}

int nova_connect(int sockfd, const struct nova_sockaddr *addr, socklen_t addrlen) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    (void)addr;
    (void)addrlen;
    if (sock == NULL) {
        return -1;
    }
    sock->state = 1;
    if (sock->dev == NULL) {
        sock->dev = nova_netdev_find("lo");
    }
    return 0;
}

int nova_listen(int sockfd, int backlog) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    (void)backlog;
    if (sock == NULL) {
        return -1;
    }
    sock->state = 2;
    return 0;
}

int nova_accept(int sockfd, struct nova_sockaddr *addr, socklen_t *addrlen) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    (void)addr;
    (void)addrlen;
    if (sock == NULL) {
        return -1;
    }
    return -1;
}

ssize_t nova_send(int sockfd, const void *buf, size_t len, int flags) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    struct nova_sk_buff *skb;
    (void)flags;

    if (sock == NULL || buf == NULL || len == 0) {
        return -1;
    }

    if (sock->dev == NULL) {
        sock->dev = nova_netdev_find("lo");
    }
    if (sock->dev == NULL || sock->dev->ops == NULL ||
        sock->dev->ops->start_xmit == NULL) {
        return -1;
    }

    skb = nova_alloc_skb(len);
    if (skb == NULL) {
        return -1;
    }
    memcpy(skb->data, buf, len);
    skb->len = len;
    skb->dev = sock->dev;

    if (nova_dev_queue_xmit(skb) != 0) {
        nova_free_skb(skb);
        return -1;
    }
    return (ssize_t)len;
}

ssize_t nova_recv(int sockfd, void *buf, size_t len, int flags) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    (void)flags;

    if (sock == NULL || buf == NULL || len == 0) {
        return -1;
    }

    memset(buf, 0, len);
    return (ssize_t)len;
}

int nova_shutdown(int sockfd, int how) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    (void)how;
    if (sock == NULL) {
        return -1;
    }
    sock->state = 0;
    return 0;
}

int nova_getsockopt(int sockfd, int level, int optname, void *optval,
                    socklen_t *optlen) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    (void)level;
    (void)optname;
    (void)optval;
    (void)optlen;
    if (sock == NULL) {
        return -1;
    }
    return 0;
}

int nova_setsockopt(int sockfd, int level, int optname, const void *optval,
                    socklen_t optlen) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    (void)level;
    (void)optname;
    (void)optval;
    (void)optlen;
    if (sock == NULL) {
        return -1;
    }
    return 0;
}

int nova_close(int sockfd) {
    struct nova_socket *sock = nova_socket_lookup(sockfd);
    if (sock == NULL) {
        return -1;
    }

    free(sock->private_data);
    free(sock);
    g_socket_table[sockfd] = NULL;
    return 0;
}

int nova_netdev_register(struct nova_net_device *dev) {
    struct nova_net_device *curr;

    if (dev == NULL || dev->name[0] == '\0') {
        return -1;
    }

    for (curr = g_nova_netdev_registry; curr != NULL; curr = curr->next) {
        if (strcmp(curr->name, dev->name) == 0) {
            return -1;
        }
    }

    dev->next = g_nova_netdev_registry;
    g_nova_netdev_registry = dev;

    if (dev->ops != NULL && dev->ops->open != NULL) {
        dev->ops->open(dev);
    }
    return 0;
}

int nova_netdev_unregister(struct nova_net_device *dev) {
    struct nova_net_device *curr = g_nova_netdev_registry;
    struct nova_net_device *prev = NULL;

    if (dev == NULL) {
        return -1;
    }

    while (curr != NULL) {
        if (curr == dev) {
            if (prev == NULL) {
                g_nova_netdev_registry = curr->next;
            } else {
                prev->next = curr->next;
            }
            curr->next = NULL;
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1;
}

struct nova_net_device *nova_netdev_find(const char *name) {
    struct nova_net_device *dev;
    if (name == NULL) {
        return NULL;
    }

    for (dev = g_nova_netdev_registry; dev != NULL; dev = dev->next) {
        if (strcmp(dev->name, name) == 0) {
            return dev;
        }
    }
    return NULL;
}

struct nova_sk_buff *nova_alloc_skb(size_t size) {
    struct nova_sk_buff *skb = calloc(1, sizeof(*skb));
    if (skb == NULL) {
        return NULL;
    }

    skb->data = calloc(1, size == 0 ? 1 : size);
    if (skb->data == NULL) {
        free(skb);
        return NULL;
    }
    skb->len = size;
    skb->headroom = 0;
    skb->tailroom = 0;
    skb->dev = NULL;
    skb->hash = 0;
    skb->protocol = 0;
    skb->priority = 0;
    skb->error = 0;
    return skb;
}

void nova_free_skb(struct nova_sk_buff *skb) {
    if (skb == NULL) {
        return;
    }
    free(skb->data);
    free(skb);
}

int nova_netif_receive_skb(struct nova_sk_buff *skb) {
    if (skb == NULL) {
        return -1;
    }

    if (skb->dev != NULL) {
        skb->dev->stats.rx_packets += 1;
        skb->dev->stats.rx_bytes += skb->len;
    }
    skb->error = 0;
    return 0;
}

int nova_dev_queue_xmit(struct nova_sk_buff *skb) {
    if (skb == NULL || skb->dev == NULL || skb->dev->ops == NULL ||
        skb->dev->ops->start_xmit == NULL) {
        return -1;
    }
    return skb->dev->ops->start_xmit(skb->dev, skb);
}

uint32_t nova_htonl(uint32_t hostlong) {
    return ((hostlong & 0x000000FFu) << 24) |
           ((hostlong & 0x0000FF00u) << 8) |
           ((hostlong & 0x00FF0000u) >> 8) |
           ((hostlong & 0xFF000000u) >> 24);
}

uint16_t nova_htons(uint16_t hostshort) {
    return (uint16_t)(((hostshort & 0x00FFu) << 8) |
                      ((hostshort & 0xFF00u) >> 8));
}

uint32_t nova_ntohl(uint32_t netlong) {
    return nova_htonl(netlong);
}

uint16_t nova_ntohs(uint16_t netshort) {
    return nova_htons(netshort);
}
