/* Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define __STDC_LIMIT_MACROS

#include "nsapi.h"
#include "mbed_interface.h"
#include "mbed_assert.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <new>
#include <stdint.h>

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/ip.h"
#include "lwip/mld6.h"
#include "lwip/dns.h"
#include "lwip/udp.h"

#include "LWIPStack.h"

LWIP::Interface *LWIP::Interface::list;

LWIP::Interface *LWIP::Interface::our_if_from_netif(struct netif *netif)
{
    return NULL;
}

static void add_dns_addr_to_dns_list_index(const u8_t addr_type, const u8_t index, struct netif *netif)
{
}

static int get_ip_addr_type(const ip_addr_t *ip_addr)
{
}

void LWIP::add_dns_addr(struct netif *lwip_netif, const char *interface_name)
{
}

nsapi_error_t LWIP::Interface::set_dhcp()
{
    return NSAPI_ERROR_OK;
}

void LWIP::Interface::netif_link_irq(struct netif *netif)
{
}

void LWIP::Interface::netif_status_irq(struct netif *netif)
{
}

void LWIP::Interface::attach(mbed::Callback<void(nsapi_event_t, intptr_t)> status_cb)
{
    client_callback = status_cb;
}

nsapi_connection_status_t LWIP::Interface::get_connection_status() const
{
    return connected;
}

char *LWIP::Interface::get_mac_address(char *buf, nsapi_size_t buflen)
{
    return buf;
}

char *LWIP::Interface::get_interface_name(char *buf)
{
    return buf;
}

nsapi_error_t LWIP::Interface::get_ip_address(SocketAddress *address)
{
    return NSAPI_ERROR_OK;
}

char *LWIP::Interface::get_ip_address(char *buf, nsapi_size_t buflen)
{
    return NULL;
}

nsapi_error_t LWIP::Interface::get_ipv6_link_local_address(SocketAddress *address)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::Interface::get_ip_address_if(const char *interface_name, SocketAddress *address)
{
    return NSAPI_ERROR_OK;
}

char *LWIP::Interface::get_ip_address_if(char *buf, nsapi_size_t buflen, const char *interface_name)
{
    return NULL;
}

nsapi_error_t LWIP::Interface::get_netmask(SocketAddress *address)
{
    return NSAPI_ERROR_OK;
}

char *LWIP::Interface::get_netmask(char *buf, nsapi_size_t buflen)
{
    return NULL;
}

nsapi_error_t LWIP::Interface::get_gateway(SocketAddress *address)
{
    return NSAPI_ERROR_OK;
}

char *LWIP::Interface::get_gateway(char *buf, nsapi_size_t buflen)
{
    return NULL;
}

LWIP::Interface::Interface() :
    hw(NULL), has_addr_state(0),
    connected(NSAPI_STATUS_DISCONNECTED),
    dhcp_started(false), dhcp_has_to_be_set(false), blocking(true), ppp_enabled(false)
{
}

nsapi_error_t LWIP::add_ethernet_interface(EMAC &emac, bool default_if, OnboardNetworkStack::Interface **interface_out)
{
    return NSAPI_ERROR_UNSUPPORTED;
}


nsapi_error_t LWIP::add_l3ip_interface(L3IP &l3ip, bool default_if, OnboardNetworkStack::Interface **interface_out)
{
    Interface *interface = new Interface();
    *interface_out = interface;
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::remove_l3ip_interface(OnboardNetworkStack::Interface **interface_out)
{
    delete *interface_out;
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::add_ppp_interface(PPP &ppp, bool default_if, OnboardNetworkStack::Interface **interface_out)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t LWIP::remove_ppp_interface(OnboardNetworkStack::Interface **interface_out)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

void LWIP::set_default_interface(OnboardNetworkStack::Interface *interface)
{
}

nsapi_error_t LWIP::Interface::bringup(bool dhcp, const char *ip, const char *netmask, const char *gw, const nsapi_ip_stack_t stack, bool block)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::Interface::bringdown()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::Interface::set_ip_address(const char *ip,
                                              const char *netmask,
                                              const char *gw,
                                              u8_t ipv6_flag)
{
    return NSAPI_ERROR_OK;
}
