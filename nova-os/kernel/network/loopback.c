/* Minimal loopback device prototype for NovaOS
 * - Registers a loopback net device named "lo"
 * - start_xmit sends packets back to nova_netif_receive_skb
 *
 * This is a small prototype for initial verification only.
 */

#include "network_stack.h"
#include <stdlib.h>
#include <string.h>

static struct nova_net_device lo_dev;

static int lo_open(struct nova_net_device *dev) {
    dev->flags |= NOVA_IFF_RUNNING | NOVA_IFF_LOOPBACK;
    return 0;
}

static int lo_stop(struct nova_net_device *dev) {
    dev->flags &= ~NOVA_IFF_RUNNING;
    return 0;
}

static int lo_start_xmit(struct nova_net_device *dev, struct nova_sk_buff *skb) {
    /* Loopback: deliver packet back to the stack. Do not consume original skb
       in a real implementation we would clone; for prototype reuse skb and
       pass it to receive callback. */
    if (!skb) return -1;
    /* Update stats */
    dev->stats.tx_packets += 1;
    dev->stats.tx_bytes += skb->len;

    /* deliver back as if received */
    dev->stats.rx_packets += 1;
    dev->stats.rx_bytes += skb->len;

    /* In a full implementation clone skb; here directly hand to receive */
    nova_netif_receive_skb(skb);
    return 0;
}

static int lo_get_stats(struct nova_net_device *dev, struct nova_net_stats *stats) {
    if (!dev || !stats) return -1;
    memcpy(stats, &dev->stats, sizeof(*stats));
    return 0;
}

static struct nova_net_device_ops lo_ops = {
    .open = lo_open,
    .stop = lo_stop,
    .start_xmit = lo_start_xmit,
    .set_mac = NULL,
    .get_stats = lo_get_stats,
};

int nova_loopback_init(void) {
    memset(&lo_dev, 0, sizeof(lo_dev));
    strncpy(lo_dev.name, "lo", sizeof(lo_dev.name)-1);
    lo_dev.mtu = 65536;
    lo_dev.flags = NOVA_IFF_LOOPBACK | NOVA_IFF_UP;
    lo_dev.ops = &lo_ops;
    return nova_netdev_register(&lo_dev);
}
