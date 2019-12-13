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
#include "RIL_stub.h"

using namespace mbed;

RIL_RadioState RIL_stub_values::radio_state = RADIO_STATE_UNAVAILABLE;
bool RIL_stub_values::bool_value = false;
RIL_Errno RIL_stub_values::err_value = RIL_E_SUCCESS;
void *RIL_stub_values::data_ptr = NULL;

RIL_stub::RIL_stub()
{

}

RIL_stub::~RIL_stub()
{

}

RIL_stub *RIL_stub::GetInstance()
{
    static RIL_stub modem;
    return &modem;
}

#ifdef __cplusplus
extern "C" {
#endif

static void onRequest(int request, void *data, size_t datalen, RIL_Token t)
{
    RIL_stub_values::data_ptr = data;
    // void (*OnRequestComplete)(RIL_Token t, RIL_Errno e, void *response, size_t responselen);
    mbed::RIL_stub::GetInstance()->_env->OnRequestComplete(t, RIL_stub_values::err_value, NULL, 0);
}

static RIL_RadioState onStateRequest()
{
    return RIL_stub_values::radio_state;
}

static const char *getVersion(void)
{
    return "RIL Simulator v12";
}

static void onCancel(RIL_Token t)
{
    mbed::RIL_stub::GetInstance()->_env->OnRequestComplete(t, RIL_stub_values::err_value, NULL, 0);
}

/**
 * RIL callbacks exposed to RILD daemon
 */
static const RIL_RadioFunctions s_callbacks = {
    12,
    onRequest,
    onStateRequest,
    NULL,
    onCancel,
    getVersion
};

const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc, char **argv)
{
    mbed::RIL_stub *inst = mbed::RIL_stub::GetInstance();
    if (RIL_stub_values::bool_value) {
        inst->_env = env;
        return &s_callbacks;
    } else {
        return NULL;
    }
}

#ifdef __cplusplus
}
#endif
