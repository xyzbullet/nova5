/* Minimal NIC stub device for NovaOS
 * - Registers a device named "nic0"
 * - start_xmit simulates transmission by updating stats and freeing skb
 *
 * This is a simple prototype for early testing; real drivers will replace it.
 */

#include "network_stack.h"
#include <stdlib.h>
#include <string.h>

static struct nova_net_device nic_dev;

static int nic_open(struct nova_net_device *dev) {
    dev->flags |= NOVA_IFF_RUNNING;
    return 0;
}

static int nic_stop(struct nova_net_device *dev) {
    dev->flags &= ~NOVA_IFF_RUNNING;
    return 0;
}

static int nic_start_xmit(struct nova_net_device *dev, struct nova_sk_buff *skb) {
    if (!dev || !skb) return -1;
    /* Simulate transmission by updating stats */
    dev->stats.tx_packets += 1;
    dev->stats.tx_bytes += skb->len;

    /* Drop packet (stub) */
    nova_free_skb(skb);
    return 0;
}

static int nic_get_stats(struct nova_net_device *dev, struct nova_net_stats *stats) {
    if (!dev || !stats) return -1;
    memcpy(stats, &dev->stats, sizeof(*stats));
    return 0;
}

static struct nova_net_device_ops nic_ops = {
    .open = nic_open,
    .stop = nic_stop,
    .start_xmit = nic_start_xmit,
    .set_mac = NULL,
    .get_stats = nic_get_stats,
};

int nova_nic_stub_init(void) {
    memset(&nic_dev, 0, sizeof(nic_dev));
    strncpy(nic_dev.name, "nic0", sizeof(nic_dev.name)-1);
    nic_dev.mtu = 1500;
    nic_dev.flags = NOVA_IFF_UP | NOVA_IFF_RUNNING;
    /* some fake MAC */
    nic_dev.mac[0] = 0x02; nic_dev.mac[1] = 0x00; nic_dev.mac[2] = 0x00;
    nic_dev.mac[3] = 0x00; nic_dev.mac[4] = 0x00; nic_dev.mac[5] = 0x01;
    nic_dev.ops = &nic_ops;
    return nova_netdev_register(&nic_dev);
}
