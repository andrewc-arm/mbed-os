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
#ifndef MBED_OS_UNITTESTS_STUBS_RILADAPTATION_STUB_H_
#define MBED_OS_UNITTESTS_STUBS_RILADAPTATION_STUB_H_

#include "RILAdaptation.h"
#include "RIL_CellularDevice.h"
#include "CellularLog.h"

namespace RILAdaptation_stub {
extern nsapi_error_t nsapi_error_value;
extern bool return_null_token;
extern RIL_Errno cb_err;
extern void *cb_response;
extern size_t cb_response_len;
extern RIL_RadioState radio_state;
extern int ril_token_id;
}

#endif /* MBED_OS_UNITTESTS_STUBS_RILADAPTATION_STUB_H_ */
