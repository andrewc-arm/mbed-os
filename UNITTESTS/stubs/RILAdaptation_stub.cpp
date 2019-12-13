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
#include "RILAdaptation_stub.h"

using namespace mbed;

nsapi_error_t RILAdaptation_stub::nsapi_error_value = NSAPI_ERROR_OK;
bool RILAdaptation_stub::return_null_token = false;
RIL_Errno RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
void *RILAdaptation_stub::cb_response = NULL;
size_t RILAdaptation_stub::cb_response_len = 0;
RIL_RadioState RILAdaptation_stub::radio_state = RADIO_STATE_ON;
int RILAdaptation_stub::ril_token_id = 0;

RILAdaptation::RILAdaptation() : _device(0), _radio_funcs(0), _ril_token_id(0)
{
}

RILAdaptation::~RILAdaptation()
{
}

void RILAdaptation::set_device(RIL_CellularDevice *device)
{
    _device = device;
}

nsapi_error_t RILAdaptation::init_ril()
{
    return RILAdaptation_stub::nsapi_error_value;
}

RIL_RadioState RILAdaptation::get_radio_state()
{
    return RILAdaptation_stub::radio_state;
}

ril_token_t *RILAdaptation::send_request(int request, void *data, size_t datalen, Callback<void(ril_token_t *, RIL_Errno, void *, size_t)> callback,
                                         rtos::Mutex *cond_mutex, rtos::ConditionVariable *cond_var)
{
    if (RILAdaptation_stub::return_null_token) {
        tr_warning("RILAdaptation::send_request, Radio funcs not available");
        return NULL;
    } else {
        RILAdaptation_stub::ril_token_id++;

        ril_token_t *token = new ril_token_t;

        token->token_id = RILAdaptation_stub::ril_token_id;
        token->request_id = request;
        token->cb = callback;
        token->cond_mutex = cond_mutex;
        token->cond_var = cond_var;

        if (callback) {
            callback(token, RILAdaptation_stub::cb_err, RILAdaptation_stub::cb_response,
                     RILAdaptation_stub::cb_response_len);
        }

        return token;
    }
}

void RILAdaptation::cancel_request(ril_token_t *token)
{
}

RILAdaptation *RILAdaptation::get_instance()
{
    static RILAdaptation ad;
    return &ad;
}

