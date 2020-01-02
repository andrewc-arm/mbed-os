/*
 * Copyright (c) 2019, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
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

#include "nsapi.h"
#include "mbed_interface.h"
#include "mbed_assert.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#include "LWIPStack.h"

LWIP::LWIP()
{
}

LWIP &LWIP::get_instance()
{
    static LWIP lwip;
    return lwip;
}


void LWIP::socket_callback(struct netconn *nc, enum netconn_evt eh, u16_t len)
{
}

#if !LWIP_IPV4 || !LWIP_IPV6
static bool all_zeros(const uint8_t *p, int len)
{
    return true;
}
#endif

static bool convert_lwip_addr_to_mbed(nsapi_addr_t *out, const ip_addr_t *in)
{
    return false;
}

static bool convert_mbed_addr_to_lwip(ip_addr_t *out, const nsapi_addr_t *in)
{
    return false;
}

void LWIP::tcpip_init_irq(void *eh)
{
}

nsapi_error_t LWIP::get_dns_server(int index, SocketAddress *address, const char *interface_name)
{
    return NSAPI_ERROR_NO_ADDRESS;
}

nsapi_error_t LWIP::add_dns_server(const SocketAddress &address, const char *interface_name)
{
	return NSAPI_ERROR_OK;
}

void LWIP::tcpip_thread_callback(void *ptr)
{
}

nsapi_error_t LWIP::call_in(int delay, mbed::Callback<void()> func)
{
    return NSAPI_ERROR_OK;
}

LWIP::call_in_callback_cb_t LWIP::get_call_in_callback()
{
    call_in_callback_cb_t cb(this, &LWIP::call_in);
    return cb;
}

const char *LWIP::get_ip_address()
{
    return NULL;
}

nsapi_error_t LWIP::get_ip_address_if(SocketAddress *address, const char *interface_name)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::socket_open(nsapi_socket_t *handle, nsapi_protocol_t proto)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::socket_close(nsapi_socket_t handle)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::socket_bind(nsapi_socket_t handle, const SocketAddress &address)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::socket_listen(nsapi_socket_t handle, int backlog)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::socket_connect(nsapi_socket_t handle, const SocketAddress &address)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::socket_accept(nsapi_socket_t server, nsapi_socket_t *handle, SocketAddress *address)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_size_or_error_t LWIP::socket_send(nsapi_socket_t handle, const void *data, nsapi_size_t size)
{
    return 0;
}

nsapi_size_or_error_t LWIP::socket_recv(nsapi_socket_t handle, void *data, nsapi_size_t size)
{
    return 0;
}

nsapi_size_or_error_t LWIP::socket_sendto(nsapi_socket_t handle, const SocketAddress &address, const void *data, nsapi_size_t size)
{
    return 0;
}

nsapi_size_or_error_t LWIP::socket_recvfrom(nsapi_socket_t handle, SocketAddress *address, void *data, nsapi_size_t size)
{
    return 0;
}

int32_t LWIP::find_multicast_member(const struct mbed_lwip_socket *s, const nsapi_ip_mreq_t *imr)
{

    return -1;
}

nsapi_error_t LWIP::setsockopt(nsapi_socket_t handle, int level, int optname, const void *optval, unsigned optlen)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t LWIP::getsockopt(nsapi_socket_t handle, int level, int optname, void *optval, unsigned *optlen)
{
    return NSAPI_ERROR_UNSUPPORTED;
}


void LWIP::socket_attach(nsapi_socket_t handle, void (*callback)(void *), void *data)
{
}

// This works as long as it's not ever set to something which corresponds to
// a macro defined as a non-integer. Eg `#define Nanostack "Foo"`
#define LWIP 0x11991199
#if MBED_CONF_NSAPI_DEFAULT_STACK == LWIP
#undef LWIP
OnboardNetworkStack &OnboardNetworkStack::get_default_instance()
{
    return LWIP::get_instance();
}
#endif
