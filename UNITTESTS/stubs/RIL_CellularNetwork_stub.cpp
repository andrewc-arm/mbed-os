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

#include "RIL_CellularNetwork.h"

using namespace mbed;

RIL_CellularNetwork::RIL_CellularNetwork(RIL_CellularDevice &device) : _rat(RAT_UNKNOWN), _int_data(-1),
    _rssi(-1), _ber(-1), _active_context(false), _connection_status_cb(0), _device(device)
{
}

RIL_CellularNetwork::~RIL_CellularNetwork()
{
}

RIL_RadioTechnology RIL_CellularNetwork::get_RIL_rat() const
{
    RIL_RadioTechnology ril_rat = RADIO_TECH_LTE;
    return ril_rat;
}

nsapi_error_t RIL_CellularNetwork::set_registration(const char *plmn)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularNetwork::get_network_registering_mode(NWRegisteringMode &mode)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularNetwork::set_attach()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularNetwork::get_attach(AttachStatus &status)
{
    status = CellularNetwork::Detached;
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularNetwork::detach()
{
    return NSAPI_ERROR_UNSUPPORTED;
}

void RIL_CellularNetwork::attach(Callback<void(nsapi_event_t, intptr_t)> status_cb)
{
    _connection_status_cb = status_cb;
}

nsapi_error_t RIL_CellularNetwork::set_access_technology_impl(RadioAccessTechnology rat)
{
    _rat = rat;
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularNetwork::set_access_technology(RadioAccessTechnology rat)
{
    return set_access_technology_impl(rat);
}

nsapi_error_t RIL_CellularNetwork::scan_plmn(operList_t &operators, int &ops_count)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularNetwork::set_ciot_optimization_config(CIoT_Supported_Opt supported_opt,
                                                                CIoT_Preferred_UE_Opt preferred_opt,
                                                                Callback<void(CIoT_Supported_Opt)> network_support_cb)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularNetwork::get_ciot_ue_optimization_config(CIoT_Supported_Opt &supported_opt,
                                                                   CIoT_Preferred_UE_Opt &preferred_opt)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularNetwork::get_ciot_network_optimization_config(CIoT_Supported_Opt &supported_network_opt)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularNetwork::get_signal_quality(int &rssi, int *ber)
{
    return NSAPI_ERROR_OK;
}

int RIL_CellularNetwork::get_3gpp_error()
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularNetwork::get_operator_params(int &format, operator_t &operator_params)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t RIL_CellularNetwork::set_registration_urc(RegistrationType type, bool on)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularNetwork::get_operator_names(operator_names_list &op_names)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

bool RIL_CellularNetwork::is_active_context(int *number_of_active_contexts, int cid)
{
    return false;
}

nsapi_error_t RIL_CellularNetwork::get_registration_params(registration_params_t &reg_params)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularNetwork::get_registration_params(RegistrationType type, registration_params_t &reg_params)
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t RIL_CellularNetwork::set_receive_period(int mode, EDRXAccessTechnology act_type, uint8_t edrx_value)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

CellularNetwork::RadioAccessTechnology RIL_CellularNetwork::RILrat_to_rat(RIL_RadioTechnology tech)
{
    RadioAccessTechnology rat = RAT_NB1;
    return rat;
}

void RIL_CellularNetwork::data_registration_state_response(ril_token_t *token, RIL_Errno err, void *response,
                                                           size_t response_len)
{
}

void RIL_CellularNetwork::network_registering_mode_response(ril_token_t *token, RIL_Errno err, void *response,
                                                            size_t response_len)
{
}

void RIL_CellularNetwork::signal_strength_response(ril_token_t *token, RIL_Errno err, void *response,
                                                   size_t response_len)
{
}

void RIL_CellularNetwork::unsolicited_response(int response_id, const void *data, size_t data_len)
{
}
