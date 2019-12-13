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

#include "RIL_CellularDevice.h"
#include "RIL_CellularNetwork.h"
using namespace mbed;

static const intptr_t cellular_properties[RIL_CellularDevice::PROPERTY_MAX] = {
    1,  // C_EREG
    0,  // C_GREG
    0,  // C_REG
    1,  // PROPERTY_IPV4_STACK
    1,  // PROPERTY_IPV6_STACK
    1,  // PROPERTY_IPV4V6_STACK
    1,  // PROPERTY_NON_IP_PDP_TYPE
};

RIL_CellularDevice::RIL_CellularDevice(RILAdaptation &ril)
    : CellularDevice(NULL),
      _ril(ril),
      _network(0)
{
    _ril.set_device(this);
    _property_array = cellular_properties;
}

RIL_CellularDevice::~RIL_CellularDevice()
{
    delete _network;
}

intptr_t RIL_CellularDevice::get_property(CellularProperty key)
{
    return _property_array[key];
}

nsapi_error_t RIL_CellularDevice::hard_power_on()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularDevice::hard_power_off()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularDevice::soft_power_on()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularDevice::soft_power_off()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularDevice::set_pin(const char *sim_pin)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularDevice::get_sim_state(SimState &state)
{
    return NSAPI_ERROR_OK;
}

CellularContext *RIL_CellularDevice::create_context(FileHandle *fh, const char *apn, bool cp_req, bool nonip_req)
{
    return NULL;
}

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN)
CellularContext *RIL_CellularDevice::create_context(UARTSerial *serial, const char *apn, PinName dcd_pin,
                                                    bool active_high, bool cp_req, bool nonip_req)
{
    return NULL;
}
#endif

RIL_CellularContext *RIL_CellularDevice::create_context_impl(const char *apn, bool cp_req, bool nonip_req)
{
    return NULL;
}

void RIL_CellularDevice::delete_context(CellularContext *context)
{

}

CellularContext *RIL_CellularDevice::get_context_list() const
{
    return NULL;
}

nsapi_error_t RIL_CellularDevice::set_baud_rate(int baud_rate)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularDevice::send_at_command(char *data, size_t data_len)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

CellularNetwork *RIL_CellularDevice::open_network(FileHandle *fh)
{
    if (!_network) {
        _network = new RIL_CellularNetwork(*this);
    }
    _network_ref_count++;
    return _network;
}

RIL_CellularNetwork *RIL_CellularDevice::open_network_impl()
{
    return NULL;
}

CellularSMS *RIL_CellularDevice::open_sms(FileHandle *fh)
{
    return NULL;
}

CellularInformation *RIL_CellularDevice::open_information(FileHandle *fh)
{
    return NULL;
}

RIL_CellularInformation *RIL_CellularDevice::open_information_impl()
{
    return NULL;
}

void RIL_CellularDevice::close_network()
{
    if (_network) {
        _network_ref_count--;
        if (_network_ref_count == 0) {
            delete _network;
            _network = NULL;
        }
    }
}

void RIL_CellularDevice::close_sms()
{
}

void RIL_CellularDevice::close_information()
{
}

void RIL_CellularDevice::set_timeout(int /*timeout*/)
{
}

void RIL_CellularDevice::modem_debug_on(bool /*on*/)
{
}

nsapi_error_t RIL_CellularDevice::init()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularDevice::is_ready()
{
    return _device_ready ? NSAPI_ERROR_OK : NSAPI_ERROR_DEVICE_ERROR;
}

void RIL_CellularDevice::set_ready_cb(Callback<void()> callback)
{
    _device_ready_cb = callback;
}

nsapi_error_t RIL_CellularDevice::set_power_save_mode(int periodic_time, int active_time)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

ATHandler *RIL_CellularDevice::get_at_handler()
{
    return NULL;
}

nsapi_error_t RIL_CellularDevice::release_at_handler(ATHandler *at_handler)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

void RIL_CellularDevice::get_radiostatus(SimState *state, bool unsolicited_resp)
{
}

// callback methods from ril/RILAdaptation
void RIL_CellularDevice::request_ack(ril_token_t */*token*t*/)
{
}

void RIL_CellularDevice::request_timed_callback(RIL_TimedCallback /*callback*/, void */*param*/,
                                                const struct timeval */*relative_time*/)
{
}

void RIL_CellularDevice::sim_status_response(ril_token_t *token, RIL_Errno err, void *response, size_t response_len)
{
}

void RIL_CellularDevice::common_response(ril_token_t *token, RIL_Errno err, void *response, size_t response_len)
{
}

void RIL_CellularDevice::unsolicited_response(int response_id, const void *data, size_t data_len)
{
}

nsapi_error_t RIL_CellularDevice::lock_and_send_request(int request_id, void *data, size_t data_len,
                                                        Callback<void(ril_token_t *, RIL_Errno, void *, size_t)> callback, int timeout)
{
    nsapi_error_t ret = NSAPI_ERROR_OK;

    //_error = RIL_CellularBase_stub::error_value;

    ril_token_t *token = _ril.send_request(request_id, data, data_len, callback, NULL, NULL);
    if (token) {
        // send_request will call response synchronously
        ret = token->response_error;
        delete token;
    } else {
        ret = NSAPI_ERROR_DEVICE_ERROR;
    }

    return ret;
}

RILAdaptation &RIL_CellularDevice::get_ril()
{
    return _ril;
}
