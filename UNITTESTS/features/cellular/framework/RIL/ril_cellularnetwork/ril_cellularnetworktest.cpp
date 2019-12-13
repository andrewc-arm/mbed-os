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

#include "gtest/gtest.h"
#include <string.h>
#include "RIL_CellularNetwork.h"
#include "RIL_CellularDevice.h"
#include "RIL_stub.h"
#include "RILAdaptation_stub.h"

using namespace mbed;

// AStyle ignored as the definition is not clear due to preprocessor usage
// *INDENT-OFF*
class TestRIL_CellularNetwork : public testing::Test {
protected:

    void SetUp()
    {
        RILAdaptation_stub::return_null_token = false;

        ril = new RILAdaptation;
        ASSERT_TRUE(ril != NULL);
        dev = new RIL_CellularDevice(*ril);
        ASSERT_TRUE(dev != NULL);
        nw = new RIL_CellularNetwork(*dev);
        ASSERT_TRUE(nw != NULL);
    }

    void TearDown()
    {
        delete nw;
        nw = NULL;

        delete dev;
        dev = NULL;

        delete ril;
        ril = NULL;
    }

    RILAdaptation *ril;
    RIL_CellularDevice *dev;
    RIL_CellularNetwork *nw;
};

class test_RIL_CellularNetwork : public RIL_CellularNetwork {
public:
    test_RIL_CellularNetwork(RIL_CellularDevice &device) : RIL_CellularNetwork(device) {};

    void unsolicited_response(int response_id, const void *data, size_t data_len)
    {
        RIL_CellularNetwork::unsolicited_response(response_id, data, data_len);
    }
};

static const intptr_t cellular_properties[RIL_CellularDevice::PROPERTY_MAX] = {
    1,  // C_EREG
    0,  // C_GREG
    0,  // C_REG
    1,  // PROPERTY_IPV4_STACK
    1,  // PROPERTY_IPV6_STACK
    1,  // PROPERTY_IPV4V6_STACK
    1,  // PROPERTY_NON_IP_PDP_TYPE
};

class test_device : public RIL_CellularDevice {
public:
    test_device(RILAdaptation &ril) : RIL_CellularDevice(ril) {
        _property_array = cellular_properties;
    }
};

static void network_callback(nsapi_event_t event, intptr_t ptr)
{

}

static void ciot_callback(CellularNetwork::CIoT_Supported_Opt opt)
{

}

// *INDENT-ON*
TEST_F(TestRIL_CellularNetwork, get_RIL_rat)
{
    RIL_RadioTechnology rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_LTE); // not defined rat, default is RADIO_TECH_LTE

    nw->set_access_technology(CellularNetwork::RAT_GSM);
    rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_GSM);

    nw->set_access_technology(CellularNetwork::RAT_UTRAN);
    rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_UMTS);

    nw->set_access_technology(CellularNetwork::RAT_EGPRS);
    rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_EDGE);

    nw->set_access_technology(CellularNetwork::RAT_HSDPA);
    rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_HSDPA);

    nw->set_access_technology(CellularNetwork::RAT_HSUPA);
    rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_HSUPA);

    nw->set_access_technology(CellularNetwork::RAT_HSDPA_HSUPA);
    rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_HSPA);

    nw->set_access_technology(CellularNetwork::RAT_NB1);
    rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_LTE);

    nw->set_access_technology(CellularNetwork::RAT_UNKNOWN);
    rat = nw->get_RIL_rat();
    ASSERT_EQ(rat, RADIO_TECH_LTE);
}

TEST_F(TestRIL_CellularNetwork, set_registration)
{
    nsapi_error_t err = nw->set_registration("1234");
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = nw->set_registration(NULL);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
}

TEST_F(TestRIL_CellularNetwork, get_network_registering_mode)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    CellularNetwork::NWRegisteringMode mode = CellularNetwork::NWModeManualAutomatic;
    nsapi_error_t err = nw->get_network_registering_mode(mode);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);
    ASSERT_EQ(mode, CellularNetwork::NWModeManualAutomatic);

    int int_data = 1;
    RILAdaptation_stub::cb_response = (void *)&int_data;
    RILAdaptation_stub::cb_response_len = 1;
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;

    err = nw->get_network_registering_mode(mode);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(mode, CellularNetwork::NWModeManual);
}

TEST_F(TestRIL_CellularNetwork, set_attach)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    nsapi_error_t err = nw->set_attach();
    ASSERT_EQ(err, NSAPI_ERROR_OK);
}

TEST_F(TestRIL_CellularNetwork, get_attach)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    CellularNetwork::AttachStatus status;
    nsapi_error_t err = nw->get_attach(status);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(status, CellularNetwork::Detached);
}

TEST_F(TestRIL_CellularNetwork, detach)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    nsapi_error_t err = nw->detach();
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, attach)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    nw->attach(network_callback);
}

TEST_F(TestRIL_CellularNetwork, set_access_technology)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    nsapi_error_t err = nw->set_access_technology(CellularNetwork::RAT_NB1);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
}

TEST_F(TestRIL_CellularNetwork, scan_plmn)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    CellularNetwork::operList_t operators;
    int ops_count;
    nsapi_error_t err = nw->scan_plmn(operators, ops_count);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, set_ciot_optimization_config)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    CellularNetwork::CIoT_Supported_Opt supported_opt;
    CellularNetwork::CIoT_Preferred_UE_Opt preferred_opt;
    nsapi_error_t err = nw->set_ciot_optimization_config(supported_opt, preferred_opt, ciot_callback);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, get_ciot_ue_optimization_config)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    CellularNetwork::CIoT_Supported_Opt supported_opt;
    CellularNetwork::CIoT_Preferred_UE_Opt preferred_opt;
    nsapi_error_t err = nw->get_ciot_ue_optimization_config(supported_opt, preferred_opt);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, get_ciot_network_optimization_config)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    CellularNetwork::CIoT_Supported_Opt supported_opt;
    nsapi_error_t err = nw->get_ciot_network_optimization_config(supported_opt);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, get_signal_quality)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    int rssi = 0;
    int ber = 0;
    nsapi_error_t err = nw->get_signal_quality(rssi, &ber);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);
    ASSERT_EQ(rssi, 0);
    ASSERT_EQ(ber, 0);


    RIL_GW_SignalStrength gsm_signal;
    gsm_signal.signalStrength = 20; // rssi
    gsm_signal.bitErrorRate = 8;

    RIL_LTE_SignalStrength_v8 lte_signal;
    lte_signal.signalStrength = 20; // rssi
    RIL_SignalStrength_v10 sig;
    sig.LTE_SignalStrength = lte_signal;
    sig.GW_SignalStrength = gsm_signal;

    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)&sig;
    RILAdaptation_stub::cb_response_len = sizeof(RIL_SignalStrength_v10);
    // set lte
    err = nw->set_access_technology(CellularNetwork::RAT_NB1);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = nw->get_signal_quality(rssi, &ber);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    // -113 + 2 * rssi; we set rssi as 20, so it should be -73
    ASSERT_EQ(rssi, -73);
    // no ber in lte
    ASSERT_EQ(ber, -1);

    // invalid rssi
    rssi = -1;
    ber = -1;
    lte_signal.signalStrength = -5; // rssi
    sig.LTE_SignalStrength = lte_signal;
    RILAdaptation_stub::cb_response = (void *)&sig;
    err = nw->get_signal_quality(rssi, &ber);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);
    ASSERT_EQ(rssi, -1);
    // no ber in lte
    ASSERT_EQ(ber, -1);

    // rssi 99
    rssi = -5;
    ber = -5;
    lte_signal.signalStrength = 99; // rssi
    sig.LTE_SignalStrength = lte_signal;
    RILAdaptation_stub::cb_response = (void *)&sig;
    err = nw->get_signal_quality(rssi, &ber);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(rssi, 0);
    // no ber in lte
    ASSERT_EQ(ber, -1);


    // Test GSM
    // set gsm
    err = nw->set_access_technology(CellularNetwork::RAT_GSM);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    err = nw->get_signal_quality(rssi, &ber);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    // -113 + 2 * rssi; we set rssi as 20, so it should be -73
    ASSERT_EQ(rssi, -73);
    // no ber in lte
    ASSERT_EQ(ber, 8);

    // invalid ber in gsm mode
    rssi = -5;
    ber = -5;
    gsm_signal.bitErrorRate = -4;
    sig.GW_SignalStrength = gsm_signal;
    RILAdaptation_stub::cb_response = (void *)&sig;
    err = nw->get_signal_quality(rssi, &ber);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);
    // -113 + 2 * rssi; we set rssi as 20, so it should be -73
    ASSERT_EQ(rssi, -5);
    // no ber in lte
    ASSERT_EQ(ber, -5);
}

TEST_F(TestRIL_CellularNetwork, get_3gpp_error)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    int err = nw->get_3gpp_error();
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, get_operator_params)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    int format;
    CellularNetwork::operator_t operator_params;
    nsapi_error_t err = nw->get_operator_params(format, operator_params);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, set_registration_urc)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;

    CellularNetwork::RegistrationType type = CellularNetwork::C_EREG;
    nsapi_error_t err = nw->set_registration_urc(type, true);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    type = CellularNetwork::C_GREG;
    err = nw->set_registration_urc(type, true);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    type = CellularNetwork::C_REG;
    err = nw->set_registration_urc(type, true);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, get_operator_names)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    CellularNetwork::operator_names_list op_names;
    nsapi_error_t err = nw->get_operator_names(op_names);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, is_active_context)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    bool ret = nw->is_active_context();
    ASSERT_EQ(ret, false);

    int contexts = -5;
    int cid = 1;
    ret = nw->is_active_context(&contexts, cid);
    ASSERT_EQ(ret, false);
    ASSERT_EQ(contexts, -5);
    ASSERT_EQ(cid, 1);
}

TEST_F(TestRIL_CellularNetwork, get_registration_params)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    CellularNetwork::registration_params_t params;
    nsapi_error_t err = nw->get_registration_params(params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    // response must be default values for CellularNetwork::registration_params_t as nothing set
    ASSERT_EQ(params._type, CellularNetwork::C_MAX);
    ASSERT_EQ(params._status, CellularNetwork::StatusNotAvailable);
    ASSERT_EQ(params._act, CellularNetwork::RAT_UNKNOWN);
    ASSERT_EQ(params._cell_id, -1);
    ASSERT_EQ(params._lac, -1);
    ASSERT_EQ(params._active_time, -1);
    ASSERT_EQ(params._periodic_tau, -1);

    CellularNetwork::RegistrationType type = CellularNetwork::C_GREG;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    type = CellularNetwork::C_REG;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    const char *response[11];
    response[0] = "1";      // registration state 0-5 from TS 27.007 10.1.20 AT+CGREG
    response[1] = "0x0FF";  // LAC if registered or NULL if not
    response[2] = "2";      // CID if registered or NULL if not
    response[3] = "14";     // RADIO_TECH_LTE = 14
    response[4] = "40";     // if registration state is 3,40 == No PDP context activated
    response[5] = "4";      // The maximum number of simultaneous Data Calls that can be established using RIL_REQUEST_SETUP_DATA_CALL.
    response[6] = "4";      // TAC, a 16-bit Tracking Area Code.
    response[7] = "4";      // CID, a 0-503 Physical Cell Identifier.
    response[8] = "4444";      // ECI, a 28-bit E-UTRAN Cell Identifier
    response[9] = "4";      // CSGID, a 27-bit Closed Subscriber Group Identity
    response[10] = "4";     // TADV, a 6-bit timing advance value.

    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)response;
    RILAdaptation_stub::cb_response_len = sizeof(response);

    type = CellularNetwork::C_EREG;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    ASSERT_EQ(params._type, CellularNetwork::C_MAX);
    ASSERT_EQ(params._status, CellularNetwork::RegisteredHomeNetwork);
    ASSERT_EQ(params._act, CellularNetwork::RAT_NB1);
    ASSERT_EQ(params._cell_id, 0);
    ASSERT_EQ(params._lac, 0);
    ASSERT_EQ(params._active_time, -1);
    ASSERT_EQ(params._periodic_tau, -1);

    // test the values we just set
    err = nw->get_registration_params(params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    // response must be default values for CellularNetwork::registration_params_t as nothing set
    ASSERT_EQ(params._type, CellularNetwork::C_MAX);
    ASSERT_EQ(params._status, CellularNetwork::RegisteredHomeNetwork);
    ASSERT_EQ(params._act, CellularNetwork::RAT_NB1);
    ASSERT_EQ(params._cell_id, 0);
    ASSERT_EQ(params._lac, 0);
    ASSERT_EQ(params._active_time, -1);
    ASSERT_EQ(params._periodic_tau, -1);

    response[7] = NULL;      // CID, a 0-503 Physical Cell Identifier.
    response[8] = "1234";      // ECI, a 28-bit E-UTRAN Cell Identifier
    RILAdaptation_stub::cb_response = (void *)response;
    RILAdaptation_stub::cb_response_len = sizeof(response);
    type = CellularNetwork::C_EREG;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    ASSERT_EQ(params._type, CellularNetwork::C_MAX);
    ASSERT_EQ(params._status, CellularNetwork::RegisteredHomeNetwork);
    ASSERT_EQ(params._act, CellularNetwork::RAT_NB1);
    ASSERT_EQ(params._cell_id, 0);
    ASSERT_EQ(params._lac, 0);
    ASSERT_EQ(params._active_time, -1);
    ASSERT_EQ(params._periodic_tau, -1);

    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    response[3] = "0";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)response;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(params._act, CellularNetwork::RAT_UNKNOWN);

    response[3] = "1";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)response;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(params._act, CellularNetwork::RAT_EGPRS);

    response[3] = "3";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)response;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(params._act, CellularNetwork::RAT_UTRAN);

    response[3] = "9";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)response;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(params._act, CellularNetwork::RAT_HSDPA);

    response[3] = "10";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)response;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(params._act, CellularNetwork::RAT_HSUPA);

    response[3] = "11";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)response;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(params._act, CellularNetwork::RAT_HSDPA_HSUPA);

    response[3] = "16";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)response;
    err = nw->get_registration_params(type, params);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(params._act, CellularNetwork::RAT_NB1);
}

TEST_F(TestRIL_CellularNetwork, set_receive_period)
{
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;

    nsapi_error_t err = nw->set_receive_period(1, CellularNetwork::EDRXGSM_EC_GSM_IoT_mode, 88);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularNetwork, unsolicited_response)
{
    test_RIL_CellularNetwork *test_nw = new test_RIL_CellularNetwork(*dev);
    test_nw->unsolicited_response(5, 0, 0);

    test_nw->unsolicited_response(RIL_UNSOL_SIGNAL_STRENGTH, 0, 0);

    delete test_nw;
}
