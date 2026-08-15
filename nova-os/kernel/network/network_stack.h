/*
 * NovaOS Networking Stack - Core Interfaces
 * Rapid Kernel Phase 3.1
 * 
 * This file defines the core networking stack interfaces for NovaOS,
 * including socket API, network device abstraction, and protocol handlers.
 */

#ifndef NOVA_NETWORK_STACK_H
#define NOVA_NETWORK_STACK_H

#include <stddef.h>
#include <stdint.h>

/* Address families */
enum nova_af {
    NOVA_AF_UNSPEC = 0,
    NOVA_AF_INET = 2,
    NOVA_AF_INET6 = 10,
    NOVA_AF_UNIX = 1,
};

/* Socket types */
enum nova_sock_type {
    NOVA_SOCK_STREAM = 1,      /* TCP */
    NOVA_SOCK_DGRAM = 2,       /* UDP */
    NOVA_SOCK_RAW = 3,         /* Raw */
    NOVA_SOCK_SEQPACKET = 5,   /* SCTP */
};

/* Protocol numbers */
enum nova_ipproto {
    NOVA_IPPROTO_IP = 0,
    NOVA_IPPROTO_ICMP = 1,
    NOVA_IPPROTO_TCP = 6,
    NOVA_IPPROTO_UDP = 17,
    NOVA_IPPROTO_IPV6 = 41,
    NOVA_IPPROTO_ICMPV6 = 58,
};

/* Socket address structures */
struct nova_sockaddr {
    uint16_t sa_family;
    char sa_data[14];
};

struct nova_sockaddr_in {
    uint16_t sin_family;
    uint16_t sin_port;
    uint32_t sin_addr;
    char sin_zero[8];
};

struct nova_sockaddr_in6 {
    uint16_t sin6_family;
    uint16_t sin6_port;
    uint32_t sin6_flowinfo;
    uint8_t sin6_addr[16];
    uint32_t sin6_scope_id;
};

/* IP address structures */
struct nova_in_addr {
    uint32_t s_addr;
};

struct nova_in6_addr {
    union {
        uint8_t __u6_addr8[16];
        uint16_t __u6_addr16[8];
        uint32_t __u6_addr32[4];
    } __u6_addr;
};

/* Socket options levels */
enum nova_sol_socket {
    NOVA_SOL_SOCKET = 1,
    NOVA_SOL_IP = 0,
    NOVA_SOL_TCP = 6,
    NOVA_SOL_UDP = 17,
};

/* Socket option names */
enum nova_so {
    NOVA_SO_DEBUG = 1,
    NOVA_SO_REUSEADDR = 2,
    NOVA_SO_TYPE = 3,
    NOVA_SO_ERROR = 4,
    NOVA_SO_DONTROUTE = 5,
    NOVA_SO_BROADCAST = 6,
    NOVA_SO_SNDBUF = 7,
    NOVA_SO_RCVBUF = 8,
    NOVA_SO_KEEPALIVE = 9,
    NOVA_SO_LINGER = 13,
    NOVA_SO_TIMESTAMP = 29,
};

/* Shutdown how values */
enum nova_shut_rd {
    NOVA_SHUT_RD = 0,
    NOVA_SHUT_WR = 1,
    NOVA_SHUT_RDWR = 2,
};

/* Network interface flags */
enum nova_iff {
    NOVA_IFF_UP = 0x1,
    NOVA_IFF_BROADCAST = 0x2,
    NOVA_IFF_LOOPBACK = 0x8,
    NOVA_IFF_RUNNING = 0x40,
    NOVA_IFF_MULTICAST = 0x1000,
};

/* Network statistics */
struct nova_net_stats {
    uint64_t rx_bytes;
    uint64_t rx_packets;
    uint64_t rx_errors;
    uint64_t rx_dropped;
    uint64_t tx_bytes;
    uint64_t tx_packets;
    uint64_t tx_errors;
    uint64_t tx_dropped;
};

/* Network interface information */
struct nova_ifreq {
    char ifr_name[16];
    union {
        struct nova_sockaddr ifru_addr;
        struct nova_sockaddr ifru_dstaddr;
        struct nova_sockaddr ifru_broadaddr;
        struct nova_sockaddr ifru_netmask;
        short ifru_flags;
        int ifru_metric;
        int ifru_mtu;
        void *ifru_data;
    } ifr_ifru;
};

#define ifr_addr ifr_ifru.ifru_addr
#define ifr_dstaddr ifr_ifru.ifru_dstaddr
#define ifr_broadaddr ifr_ifru.ifru_broadaddr
#define ifr_netmask ifr_ifru.ifru_netmask
#define ifr_flags ifr_ifru.ifru_flags
#define ifr_metric ifr_ifru.ifru_metric
#define ifr_mtu ifr_ifru.ifru_mtu
#define ifr_data ifr_ifru.ifru_data

/* Forward declarations */
struct nova_socket;
struct nova_net_device;
struct nova_sk_buff;

/* Socket operations */
struct nova_socket_ops {
    int (*create)(struct nova_socket *sock);
    int (*bind)(struct nova_socket *sock, const struct nova_sockaddr *addr, 
                socklen_t addrlen);
    int (*connect)(struct nova_socket *sock, const struct nova_sockaddr *addr,
                   socklen_t addrlen);
    int (*listen)(struct nova_socket *sock, int backlog);
    int (*accept)(struct nova_socket *sock, struct nova_sockaddr *addr,
                  socklen_t *addrlen);
    ssize_t (*send)(struct nova_socket *sock, const void *buf, size_t len,
                    int flags);
    ssize_t (*recv)(struct nova_socket *sock, void *buf, size_t len, int flags);
    int (*shutdown)(struct nova_socket *sock, int how);
    int (*getsockopt)(struct nova_socket *sock, int level, int optname,
                      void *optval, socklen_t *optlen);
    int (*setsockopt)(struct nova_socket *sock, int level, int optname,
                      const void *optval, socklen_t optlen);
    int (*close)(struct nova_socket *sock);
};

/* Socket structure */
struct nova_socket {
    enum nova_af family;
    enum nova_sock_type type;
    int protocol;
    int state;
    int flags;
    struct nova_socket_ops *ops;
    void *private_data;
    struct nova_net_device *dev;
};

/* Packet buffer (skb) for network data */
struct nova_sk_buff {
    uint8_t *data;
    size_t len;
    size_t headroom;
    size_t tailroom;
    struct nova_net_device *dev;
    uint32_t hash;
    uint16_t protocol;
    uint8_t priority;
    int error;
};

/* Network device operations */
struct nova_net_device_ops {
    int (*open)(struct nova_net_device *dev);
    int (*stop)(struct nova_net_device *dev);
    int (*start_xmit)(struct nova_net_device *dev, struct nova_sk_buff *skb);
    int (*set_mac)(struct nova_net_device *dev, const uint8_t *mac);
    int (*get_stats)(struct nova_net_device *dev, struct nova_net_stats *stats);
};

/* Network device structure */
struct nova_net_device {
    char name[16];
    uint8_t mac[6];
    uint32_t mtu;
    uint32_t flags;
    enum nova_af family;
    struct nova_net_device_ops *ops;
    struct nova_net_stats stats;
    void *private_data;
    struct nova_net_device *next;
};

/* TCP/IP stack functions */
int nova_network_init(void);
int nova_network_shutdown(void);

/* Socket API */
int nova_socket(int domain, int type, int protocol);
int nova_bind(int sockfd, const struct nova_sockaddr *addr, socklen_t addrlen);
int nova_connect(int sockfd, const struct nova_sockaddr *addr, socklen_t addrlen);
int nova_listen(int sockfd, int backlog);
int nova_accept(int sockfd, struct nova_sockaddr *addr, socklen_t *addrlen);
ssize_t nova_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t nova_recv(int sockfd, void *buf, size_t len, int flags);
int nova_shutdown(int sockfd, int how);
int nova_getsockopt(int sockfd, int level, int optname, void *optval,
                    socklen_t *optlen);
int nova_setsockopt(int sockfd, int level, int optname, const void *optval,
                    socklen_t optlen);
int nova_close(int sockfd);

/* Network device management */
int nova_netdev_register(struct nova_net_device *dev);
int nova_netdev_unregister(struct nova_net_device *dev);
struct nova_net_device *nova_netdev_find(const char *name);

/* Packet buffer management */
struct nova_sk_buff *nova_alloc_skb(size_t size);
void nova_free_skb(struct nova_sk_buff *skb);
int nova_netif_receive_skb(struct nova_sk_buff *skb);
int nova_dev_queue_xmit(struct nova_sk_buff *skb);

/* Address conversion helpers */
uint32_t nova_htonl(uint32_t hostlong);
uint16_t nova_htons(uint16_t hostshort);
uint32_t nova_ntohl(uint32_t netlong);
uint16_t nova_ntohs(uint16_t netshort);

/* IOCTL commands for network devices */
#define NOVA_SIOCGIFADDR 0x8913
#define NOVA_SIOCGIFNETMASK 0x891B
#define NOVA_SIOCGIFBRDADDR 0x8919
#define NOVA_SIOCGIFFLAGS 0x8913
#define NOVA_SIOCSIFFLAGS 0x8914
#define NOVA_SIOCGIFMTU 0x8921
#define NOVA_SIOCSIFMTU 0x8922

#endif /* NOVA_NETWORK_STACK_H */
