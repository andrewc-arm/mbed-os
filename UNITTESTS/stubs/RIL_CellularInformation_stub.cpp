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

#include "RIL_CellularInformation.h"

using namespace mbed;

RIL_CellularInformation::RIL_CellularInformation(RIL_CellularDevice &device) : _type(IMEI), _buf(0), _device(device)
{
}

RIL_CellularInformation::~RIL_CellularInformation()
{
    delete [] _buf;
    _buf = NULL;
}

nsapi_error_t RIL_CellularInformation::get_manufacturer(char *buf, size_t buf_size)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularInformation::get_model(char *buf, size_t buf_size)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularInformation::get_iccid(char *buf, size_t buf_size)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularInformation::get_revision(char *buf, size_t buf_size)
{
    return NSAPI_ERROR_OK;;
}

nsapi_error_t RIL_CellularInformation::get_imsi(char *buf, size_t buf_size)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularInformation::get_info(int request_id, char *buf, size_t buf_size)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularInformation::get_serial_number(char *buf, size_t buf_size, SerialNumberType type)
{
    return NSAPI_ERROR_OK;
}

void RIL_CellularInformation::request_device_identity_response(ril_token_t *token, RIL_Errno err, void *response,
                                                               size_t response_len)
{
}

void RIL_CellularInformation::info_request_response(ril_token_t *token, RIL_Errno err, void *response, size_t response_len)
{
}

void RIL_CellularInformation::unsolicited_response(int response_id, const void *data, size_t data_len)
{
}
