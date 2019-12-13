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

#include "RIL_CellularContext.h"

using namespace mbed;

RIL_CellularContext::RIL_CellularContext(RIL_CellularDevice &device, const char *apn, bool cp_req, bool nonip_req) :
    _l3ip_driver(0), _ifname(0), _is_connected(false), _is_running(false), _current_op(OP_INVALID),
    _final_op(OP_CONNECT), _current_state(OP_INVALID), _stack_interface(0),
    _connect_state(CS_INVALID), _set_registration_complete(false)
{
    _next = NULL;
}

RIL_CellularContext::~RIL_CellularContext()
{
}

CellularDevice *RIL_CellularContext::get_device() const
{
    return NULL;
}

nsapi_error_t RIL_CellularContext::set_blocking(bool blocking)
{
    return NSAPI_ERROR_OK;
}

NetworkStack *RIL_CellularContext::get_stack()
{
    return _stack;
}

nsapi_error_t RIL_CellularContext::get_ip_address(SocketAddress *address)
{
    address->set_ip_address("10.11.12.13");
}

const char *RIL_CellularContext::get_ip_address()
{
    return "10.11.12.13";
}

char *RIL_CellularContext::get_interface_name(char *interface_name)
{
    return NULL;
}

void RIL_CellularContext::attach(mbed::Callback<void(nsapi_event_t, intptr_t)> status_cb)
{
    _status_cb = status_cb;
}

nsapi_error_t RIL_CellularContext::connect()
{
    return NSAPI_ERROR_OK;
}

void RIL_CellularContext::do_connect_with_retry()
{
    CellularContext::do_connect_with_retry();
}

nsapi_error_t RIL_CellularContext::do_initial_attach()
{
    return NSAPI_ERROR_OK;
}

void RIL_CellularContext::request_set_initial_attach_apn_response(ril_token_t *token, RIL_Errno err, void *response, size_t response_len)
{
}

void RIL_CellularContext::do_connect()
{
}

L3IP *RIL_CellularContext::get_L3IP_driver()
{
    return NULL;
}

void RIL_CellularContext::delete_L3IP_driver()
{
}

void RIL_CellularContext::create_interface_and_connect()
{
}

nsapi_error_t RIL_CellularContext::disconnect()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularContext::check_operation(nsapi_error_t err, ContextOperation op)
{
    return NSAPI_ERROR_OK;
}

uint32_t RIL_CellularContext::get_timeout_for_operation(ContextOperation op) const
{
    uint32_t timeout = NETWORK_TIMEOUT; // default timeout is 30 minutes as registration and attach may take time
    if (op == OP_SIM_READY || op == OP_DEVICE_READY) {
        timeout = DEVICE_TIMEOUT; // use 5 minutes for device ready and sim
    }
    return timeout;
}

bool RIL_CellularContext::is_connected()
{
    return _is_connected;
}

void RIL_CellularContext::set_plmn(const char *plmn)
{
}

void RIL_CellularContext::set_sim_pin(const char *sim_pin)
{
}

nsapi_error_t RIL_CellularContext::connect(const char *sim_pin, const char *apn, const char *uname, const char *pwd)
{
    return NSAPI_ERROR_OK;
}

void RIL_CellularContext::set_credentials(const char *apn, const char *uname, const char *pwd)
{
    _apn = apn;
    _uname = uname;
    _pwd = pwd;
}

nsapi_error_t RIL_CellularContext::get_netmask(SocketAddress *address)
{
    address->set_ip_address("255.255.255.255");
}

const char *RIL_CellularContext::get_netmask()
{
    return "255.255.255.255";
}

nsapi_error_t RIL_CellularContext::get_gateway(SocketAddress *address)
{
    address->set_ip_address("10.11.12.99");
}

const char *RIL_CellularContext::get_gateway()
{
    return "10.11.12.99";
}

nsapi_error_t RIL_CellularContext::get_pdpcontext_params(pdpContextList_t &params_list)
{
    // This will return only the current context params.
    pdpcontext_params_t *params = NULL;
    params = params_list.add_new();

    strncpy(params->apn, _apn, sizeof(params->apn));
    strncpy(params->local_addr, get_ip_address(), sizeof(params->local_addr));
    strncpy(params->local_subnet_mask, get_netmask(), sizeof(params->local_subnet_mask));
    strncpy(params->gateway_addr, get_gateway(), sizeof(params->gateway_addr));

    strncpy(params->dns_primary_addr, "1.2.3.4", sizeof(params->dns_primary_addr));
    strncpy(params->dns_secondary_addr, "5.6.7.8", sizeof(params->dns_secondary_addr));

    params->p_cscf_prim_addr[0] = '\0';
    params->p_cscf_sec_addr[0] = '\0';
    params->cid = _cid;
    params->bearer_id = -1;
    params->im_signalling_flag = -1;
    params->lipa_indication = -1;
    params->ipv4_mtu = _mtu;
    params->wlan_offload = -1;
    params->local_addr_ind = -1;
    params->non_ip_mtu = _mtu;
    params->serving_plmn_rate_control_value = -1;
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularContext::get_rate_control(CellularContext::RateControlExceptionReports &reports,
                                                    CellularContext::RateControlUplinkTimeUnit &time_unit, int &uplink_rate)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularContext::get_apn_backoff_timer(int &backoff_timer)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularContext::set_device_ready()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularContext::set_sim_ready()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularContext::register_to_network()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularContext::attach_to_network()
{
    return NSAPI_ERROR_OK;
}

void RIL_CellularContext::set_file_handle(FileHandle */*fh*/)
{
}

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN)
void RIL_CellularContext::set_file_handle(UARTSerial */*serial*/, PinName /*dcd_pin*/, bool /*active_high*/)
{
}
#endif

ControlPlane_netif *RIL_CellularContext::get_cp_netif()
{
    return NULL;
}

void RIL_CellularContext::set_state(cell_callback_data_t *data, cellular_connection_status_t st)
{
}

void RIL_CellularContext::cellular_callback(nsapi_event_t ev, intptr_t ptr)
{
}

void RIL_CellularContext::enable_hup(bool /*enable*/)
{
}

void RIL_CellularContext::request_setup_data_call_response(ril_token_t *token, RIL_Errno err, void *response,
                                                           size_t response_len)
{
}

RIL_CellularDevice::CellularProperty RIL_CellularContext::pdp_type_t_to_cellular_property(pdp_type_t pdp_type)
{
    RIL_CellularDevice::CellularProperty prop = RIL_CellularDevice::PROPERTY_IPV4_PDP_TYPE;
    return prop;
}

nsapi_ip_stack_t RIL_CellularContext::pdp_type_to_ip_stack(pdp_type_t pdp_type)
{
    return (nsapi_ip_stack_t)pdp_type;
}

void RIL_CellularContext::call_network_cb(nsapi_connection_status_t status)
{
}

void RIL_CellularContext::stack_status_cb(nsapi_event_t ev, intptr_t ptr)
{
}

nsapi_connection_status_t RIL_CellularContext::get_connection_status() const
{
    return _connect_status;
}

void RIL_CellularContext::request_deactivate_data_call_response(ril_token_t *token, RIL_Errno err, void *response,
                                                                size_t response_len)
{
}

void RIL_CellularContext::unsolicited_response(int response_id, const void *data, size_t data_len)
{
}

