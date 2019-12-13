/*
 * Copyright (c) , Arm Limited and affiliates.
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

#include "CellularDevice_stub.h"
#include "events/EventQueue.h"
#include "CellularUtil.h"

using namespace mbed;

int CellularDevice_stub::connect_counter = -1;
nsapi_error_t CellularDevice_stub::nsapi_error = NSAPI_ERROR_OK;
bool CellularDevice_stub::create_in_get_default = false;
uint16_t CellularDevice_stub::retry_timeout_array[CELLULAR_RETRY_ARRAY_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int CellularDevice_stub::retry_array_length = 0;

CellularDevice::CellularDevice(FileHandle *fh) : _network_ref_count(0),
#if MBED_CONF_CELLULAR_USE_SMS
    _sms_ref_count(0),
#endif //MBED_CONF_CELLULAR_USE_SMS
    _info_ref_count(0), _fh(fh), _queue(10 * EVENTS_EVENT_SIZE), _state_machine(0),
    _nw(0), _status_cb(0), _property_array(0)
{
    set_plmn(0);
    set_sim_pin(0);
}

CellularDevice::~CellularDevice()
{

}

events::EventQueue *CellularDevice::get_queue()
{
    return &_queue;
}

void CellularDevice::set_plmn(char const *plmn)
{
    if (plmn) {
        strncpy(_plmn, plmn, sizeof(_plmn));
        _plmn[sizeof(_plmn) - 1] = '\0';
    } else {
        memset(_plmn, 0, sizeof(_plmn));
    }
}

void CellularDevice::set_sim_pin(char const *sim_pin)
{
    if (sim_pin) {
        strncpy(_sim_pin, sim_pin, sizeof(_sim_pin));
        _sim_pin[sizeof(_sim_pin) - 1] = '\0';
    } else {
        memset(_sim_pin, 0, sizeof(_sim_pin));
    }
}

CellularContext *CellularDevice::get_context_list() const
{
    return NULL;
}

void CellularDevice::get_retry_timeout_array(uint16_t *timeout, int &array_len) const
{
    array_len = CellularDevice_stub::retry_array_length;

    if (CellularDevice_stub::retry_array_length == 0) {
        timeout = 0;
    } else {
        timeout = CellularDevice_stub::retry_timeout_array;
    }
}

nsapi_error_t CellularDevice::set_device_ready()
{
    if (CellularDevice_stub::connect_counter == 0) {
        return NSAPI_ERROR_ALREADY;
    } else  if (CellularDevice_stub::connect_counter == 1) {
        CellularDevice_stub::connect_counter--;
        return NSAPI_ERROR_IN_PROGRESS;
    } else if (CellularDevice_stub::connect_counter == 2) {
        CellularDevice_stub::connect_counter--;
        return NSAPI_ERROR_OK;
    }

    return NSAPI_ERROR_OK;
}

nsapi_error_t CellularDevice::set_sim_ready()
{
    if (CellularDevice_stub::connect_counter == 0) {
        return NSAPI_ERROR_ALREADY;
    } else  if (CellularDevice_stub::connect_counter == 1) {
        CellularDevice_stub::connect_counter--;
        return NSAPI_ERROR_IN_PROGRESS;
    } else if (CellularDevice_stub::connect_counter == 2) {
        CellularDevice_stub::connect_counter--;
        return NSAPI_ERROR_OK;
    }

    return NSAPI_ERROR_OK;
}

nsapi_error_t CellularDevice::register_to_network()
{
    if (CellularDevice_stub::connect_counter == 0) {
        return NSAPI_ERROR_ALREADY;
    } else  if (CellularDevice_stub::connect_counter == 1) {
        CellularDevice_stub::connect_counter--;
        return NSAPI_ERROR_IN_PROGRESS;
    } else if (CellularDevice_stub::connect_counter == 2) {
        CellularDevice_stub::connect_counter--;
        return NSAPI_ERROR_OK;
    }

    return NSAPI_ERROR_OK;
}

nsapi_error_t CellularDevice::attach_to_network()
{
    if (CellularDevice_stub::connect_counter == 0) {
        return NSAPI_ERROR_ALREADY;
    } else  if (CellularDevice_stub::connect_counter == 1) {
        CellularDevice_stub::connect_counter--;
        return NSAPI_ERROR_IN_PROGRESS;
    } else if (CellularDevice_stub::connect_counter == 2) {
        CellularDevice_stub::connect_counter--;
        return NSAPI_ERROR_OK;
    }

    return NSAPI_ERROR_OK;
}

nsapi_error_t CellularDevice::set_pin(const char *sim_pin)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t CellularDevice::get_sim_state(SimState &state)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t CellularDevice::shutdown()
{
    return NSAPI_ERROR_OK;
}

void CellularDevice::cellular_callback(nsapi_event_t ev, intptr_t ptr, CellularContext *ctx)
{
}

nsapi_error_t CellularDevice::clear()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t CellularDevice::send_at_command(char *data, size_t data_len)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

const char *CellularDevice::get_plmn() const
{
    if (strlen(_plmn)) {
        return _plmn;
    } else {
        return NULL;
    }
}
