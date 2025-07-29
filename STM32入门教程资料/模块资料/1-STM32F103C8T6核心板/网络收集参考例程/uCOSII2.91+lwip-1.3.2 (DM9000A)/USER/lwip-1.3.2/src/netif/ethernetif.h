#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "err.h"
#include "netif.h"

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);
struct netif *ethernetif_register(void);
int ethernetif_poll(void);
void Set_MAC_Address(unsigned char* macadd);

#endif 
