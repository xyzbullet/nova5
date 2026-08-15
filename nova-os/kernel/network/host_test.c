#include "network_stack.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    struct nova_net_device *lo;
    struct nova_net_device *nic;
    struct nova_sk_buff *skb;
    int fd;

    nova_network_init();
    assert(nova_loopback_init() == 0);
    lo = nova_netdev_find("lo");
    assert(lo != NULL);
    assert((lo->flags & NOVA_IFF_LOOPBACK) != 0);

    fd = nova_socket(NOVA_AF_INET, NOVA_SOCK_STREAM, NOVA_IPPROTO_TCP);
    assert(fd >= 0);
    assert(nova_bind(fd, NULL, 0) == 0);
    assert(nova_connect(fd, NULL, 0) == 0);

    skb = nova_alloc_skb(32);
    assert(skb != NULL);
    memset(skb->data, 'A', 32);
    skb->len = 32;
    skb->dev = lo;
    assert(nova_dev_queue_xmit(skb) == 0);
    assert(lo->stats.rx_packets >= 1);
    assert(nova_close(fd) == 0);

    assert(nova_nic_stub_init() == 0);
    nic = nova_netdev_find("nic0");
    assert(nic != NULL);
    assert((nic->flags & NOVA_IFF_UP) != 0);

    skb = nova_alloc_skb(20);
    assert(skb != NULL);
    memset(skb->data, 'B', 20);
    skb->len = 20;
    skb->dev = nic;
    assert(nova_dev_queue_xmit(skb) == 0);
    assert(nic->stats.tx_packets >= 1);

    puts("NovaOS network stack prototype host tests passed.");
    return 0;
}
