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
#include "RIL_CellularContext.h"
#include "RIL_stub.h"
#include "RILAdaptation_stub.h"
#include "FileHandle_stub.h"
#include "CellularDevice_stub.h"
#include "Semaphore_stub.h"
#include "equeue_stub.h"

using namespace mbed;
using namespace events;

// AStyle ignored as the definition is not clear due to preprocessor usage
// *INDENT-OFF*
class TestRIL_CellularContext : public testing::Test {
protected:

    void SetUp()
    {
        RILAdaptation_stub::return_null_token = false;
        RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
        RILAdaptation_stub::cb_response = 0;
        RILAdaptation_stub::cb_response_len = 0;
        RILAdaptation_stub::nsapi_error_value = NSAPI_ERROR_OK;
        RILAdaptation_stub::radio_state = RADIO_STATE_ON;
        RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
        CellularDevice_stub::connect_counter = -1;
        Semaphore_stub::acquire_return_value = false;

        ril = new RILAdaptation();
        ASSERT_TRUE(ril != NULL);
        dev =  new RIL_CellularDevice(*ril);
        ASSERT_TRUE(dev != NULL);
        ctx = new RIL_CellularContext(*dev);
        ASSERT_TRUE(ctx != NULL);
    }

    void TearDown()
    {
        delete ctx;
        ctx = NULL;

        delete dev;
        dev = NULL;

        delete ril;
        ril = NULL;
    }

    RILAdaptation *ril;
    RIL_CellularDevice *dev;
    RIL_CellularContext *ctx;
};

class test_context : public RIL_CellularContext {
public:
    test_context(RIL_CellularDevice &device) : RIL_CellularContext(device)
    {
    }

    virtual void cellular_callback(nsapi_event_t ev, intptr_t ptr)
    {
        RIL_CellularContext::cellular_callback(ev, ptr);
    }

    void unsolicited_response(int response_id, const void *data, size_t data_len)
    {
        RIL_CellularContext::unsolicited_response(response_id, data, data_len);
    }

    void enable_hup(bool enable)
    {
        RIL_CellularContext::enable_hup(enable);
    }
};

static int callback_counter = 0;
static int connect_event = 0;
static RIL_Data_Call_Response_v11 resp;
static const char *IPV4_STR = "IP";
static const char *IFNAME_STR = "rmnet";
static void ctx_callback(nsapi_event_t event, intptr_t ptr)
{
    callback_counter++;
    if (event == NSAPI_EVENT_CONNECTION_STATUS_CHANGE && ptr == NSAPI_STATUS_CONNECTING) {
        if (connect_event) {
            resp.status             = PDP_FAIL_NONE;
            resp.suggestedRetryTime = 0;
            // cid will be assigned below
            resp.active             = 2;
            resp.type               = (char *)IPV4_STR;
            resp.ifname             = (char *)IFNAME_STR;
            resp.addresses          = (char *)"10.0.0.1";
            resp.dnses              = (char *)"8.8.8.8";
            resp.gateways           = (char *)"10.0.0.99";
            resp.pcscf              = (char *)"";
            resp.mtu                = 1500;

            RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
            RILAdaptation_stub::cb_response = &resp;
            RILAdaptation_stub::cb_response_len = sizeof(resp);
        }
    }
}

static void ready_callback(void)
{
}

// *INDENT-ON*
TEST_F(TestRIL_CellularContext, create)
{
    delete ctx;

    ctx = new RIL_CellularContext(*dev, "internet");
    delete ctx;

    ctx = new RIL_CellularContext(*dev, "internet", true);
    delete ctx;

    ctx = new RIL_CellularContext(*dev, "internet", true, true);
    delete ctx;
    ctx = NULL;
}

TEST_F(TestRIL_CellularContext, set_blocking)
{
    nsapi_error_t err = ctx->set_blocking(false);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = ctx->set_blocking(true);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
}

TEST_F(TestRIL_CellularContext, get_stack)
{
    NetworkStack *stack = ctx->get_stack();
    ASSERT_TRUE(stack);

    stack = ctx->get_stack();
    ASSERT_TRUE(stack);
}

TEST_F(TestRIL_CellularContext, get_ip_address)
{
    const char *ip = ctx->get_ip_address();
    ASSERT_TRUE(ip);
    ASSERT_EQ(strlen(ip), 0);
}

TEST_F(TestRIL_CellularContext, get_interface_name)
{
    char interface_name[50];
    interface_name[0] = '\0';

    const char *ifname = ctx->get_interface_name(interface_name);
    ASSERT_TRUE(ifname == NULL);
    ASSERT_EQ(strlen(interface_name), 0);
}

TEST_F(TestRIL_CellularContext, attach)
{
    ctx->attach(0);
    ctx->attach(ctx_callback);
}

TEST_F(TestRIL_CellularContext, set_plmn_sim)
{
    ctx->set_plmn(0);
    ctx->set_plmn("22432");

    ctx->set_sim_pin(0);
    ctx->set_sim_pin("1234");
}

TEST_F(TestRIL_CellularContext, set_credentials)
{
    ctx->set_credentials(0);
    ctx->set_credentials("internet");
    ctx->set_credentials("internet", "uname");
    ctx->set_credentials("internet", "uname", "pass");
}

TEST_F(TestRIL_CellularContext, get_netmask_gateway)
{
    const char *nm = ctx->get_netmask();
    ASSERT_TRUE(nm);
    ASSERT_STREQ(nm, "255.255.255.255");

    const char *gw = ctx->get_gateway();
    ASSERT_TRUE(gw);
    ASSERT_EQ(strlen(gw), 0);
}

TEST_F(TestRIL_CellularContext, get_connection_status)
{
    nsapi_connection_status_t st = ctx->get_connection_status();
    ASSERT_EQ(st, NSAPI_STATUS_DISCONNECTED);
}

TEST_F(TestRIL_CellularContext, get_pdpcontext_params)
{
    CellularContext::pdpContextList_t ctx_list;
    nsapi_error_t err = ctx->get_pdpcontext_params(ctx_list);
    CellularContext::pdpcontext_params_t *params = ctx_list.get_head();

    ASSERT_EQ(err, NSAPI_ERROR_OK);
    EXPECT_TRUE(params != NULL);
    EXPECT_TRUE(params->next == NULL);
    ASSERT_EQ(params->cid,  -1);
    ASSERT_EQ(params->bearer_id, -1);
    ASSERT_EQ(params->im_signalling_flag, -1);
    ASSERT_EQ(params->lipa_indication,  -1);
    ASSERT_EQ(params->ipv4_mtu, 1500);
    ASSERT_EQ(params->wlan_offload, -1);
    ASSERT_EQ(params->local_addr_ind, -1);
    ASSERT_EQ(params->non_ip_mtu, 1500);
    ASSERT_EQ(params->serving_plmn_rate_control_value,  -1);
    ASSERT_STREQ(params->apn, "");
    ASSERT_STREQ(params->local_addr, "");
    ASSERT_STREQ(params->local_subnet_mask, "255.255.255.255");
    ASSERT_STREQ(params->gateway_addr, "");
    ASSERT_STREQ(params->dns_primary_addr, "");
    ASSERT_STREQ(params->dns_secondary_addr, "");
    ASSERT_STREQ(params->p_cscf_prim_addr, "");
    ASSERT_STREQ(params->p_cscf_sec_addr, "");
}

TEST_F(TestRIL_CellularContext, get_rate_control)
{
    CellularContext::RateControlExceptionReports reports;
    CellularContext::RateControlUplinkTimeUnit time_unit;
    int uplink_rate;
    nsapi_error_t err = ctx->get_rate_control(reports, time_unit, uplink_rate);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularContext, get_apn_backoff_timer)
{
    int backoff_timer;
    nsapi_error_t err = ctx->get_apn_backoff_timer(backoff_timer);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularContext, set_file_handle)
{
    FileHandle_stub fh;
    ctx->set_file_handle(&fh);

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN)
    UARTSerial *serial = new UARTSerial(NC, NC);
    ctx->set_file_handle(serial);
    ctx->set_file_handle(serial, NC);
    ctx->set_file_handle(serial, NC, true);
    delete serial;
#endif
}

TEST_F(TestRIL_CellularContext, get_cp_netif)
{
    ControlPlane_netif *netif = ctx->get_cp_netif();
    ASSERT_TRUE(netif == NULL);
}

TEST_F(TestRIL_CellularContext, get_device)
{
    CellularDevice *dev2 = ctx->get_device();
    ASSERT_TRUE(dev2);
}

TEST_F(TestRIL_CellularContext, pdp_type_to_string)
{
    ASSERT_STREQ("", RIL_CellularContext::pdp_type_to_string(DEFAULT_PDP_TYPE));
    ASSERT_STREQ("IP", RIL_CellularContext::pdp_type_to_string(IPV4_PDP_TYPE));
    ASSERT_STREQ("IPV6", RIL_CellularContext::pdp_type_to_string(IPV6_PDP_TYPE));
    ASSERT_STREQ("IPV4V6", RIL_CellularContext::pdp_type_to_string(IPV4V6_PDP_TYPE));
    ASSERT_STREQ("Non-IP", RIL_CellularContext::pdp_type_to_string(NON_IP_PDP_TYPE));
}

TEST_F(TestRIL_CellularContext, pdp_type_t_to_cellular_property)
{
    RIL_CellularDevice::CellularProperty prop = RIL_CellularContext::pdp_type_t_to_cellular_property(IPV6_PDP_TYPE);
    ASSERT_EQ(prop, RIL_CellularDevice::PROPERTY_IPV6_PDP_TYPE);

    prop = RIL_CellularContext::pdp_type_t_to_cellular_property(IPV4V6_PDP_TYPE);
    ASSERT_EQ(prop, RIL_CellularDevice::PROPERTY_IPV4V6_PDP_TYPE);

    prop = RIL_CellularContext::pdp_type_t_to_cellular_property(NON_IP_PDP_TYPE);
    ASSERT_EQ(prop, RIL_CellularDevice::PROPERTY_NON_IP_PDP_TYPE);

    prop = RIL_CellularContext::pdp_type_t_to_cellular_property((pdp_type_t)55);
    ASSERT_EQ(prop, RIL_CellularDevice::PROPERTY_IPV4_PDP_TYPE);
}

TEST_F(TestRIL_CellularContext, pdp_type_to_ip_stack)
{
    nsapi_ip_stack_t stack = RIL_CellularContext::pdp_type_to_ip_stack(DEFAULT_PDP_TYPE);
    ASSERT_EQ(stack, DEFAULT_STACK);

    stack = RIL_CellularContext::pdp_type_to_ip_stack(IPV4_PDP_TYPE);
    ASSERT_EQ(stack, IPV4_STACK);

    stack = RIL_CellularContext::pdp_type_to_ip_stack(IPV6_PDP_TYPE);
    ASSERT_EQ(stack, IPV6_STACK);

    stack = RIL_CellularContext::pdp_type_to_ip_stack(IPV4V6_PDP_TYPE);
    ASSERT_EQ(stack, IPV4V6_STACK);

    stack = RIL_CellularContext::pdp_type_to_ip_stack(NON_IP_PDP_TYPE); // this should assert
    ASSERT_EQ(stack, 4);
}

TEST_F(TestRIL_CellularContext, set_device_ready)
{
    test_context *test_ctx = new test_context(*dev);

    CellularDevice_stub::connect_counter = 2;
    Semaphore_stub::acquire_return_value = true;
    nsapi_error_t err = test_ctx->set_device_ready();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx->set_device_ready();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx->set_device_ready();
    ASSERT_EQ(err, NSAPI_ERROR_ALREADY);

    CellularDevice_stub::connect_counter = 5;
    Semaphore_stub::acquire_return_value = false;
    err = test_ctx->set_device_ready();
    ASSERT_EQ(err, NSAPI_ERROR_TIMEOUT);

    callback_counter = 0;
    cell_callback_data_t cb_data;
    test_ctx->attach(ctx_callback);
    test_ctx->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    delete test_ctx;

    // test non blocking
    RIL_CellularDevice *dev2 =  new RIL_CellularDevice(*ril);
    test_context *test_ctx2 = new test_context(*dev2);

    err = test_ctx2->set_blocking(false);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx2->set_device_ready();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx2->set_device_ready();
    ASSERT_EQ(err, NSAPI_ERROR_BUSY);

    callback_counter = 0;
    test_ctx2->attach(ctx_callback);
    test_ctx2->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);

    ASSERT_EQ(callback_counter, 1);

    err = test_ctx2->set_device_ready();
    ASSERT_EQ(err, NSAPI_ERROR_ALREADY);

    delete test_ctx2;
    delete dev2;
}

TEST_F(TestRIL_CellularContext, set_sim_ready)
{
    test_context *test_ctx = new test_context(*dev);

    CellularDevice_stub::connect_counter = 2;
    Semaphore_stub::acquire_return_value = true;
    nsapi_error_t err = test_ctx->set_sim_ready();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx->set_sim_ready();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx->set_sim_ready();
    ASSERT_EQ(err, NSAPI_ERROR_ALREADY);

    CellularDevice_stub::connect_counter = 5;
    Semaphore_stub::acquire_return_value = false;
    err = test_ctx->set_sim_ready();
    ASSERT_EQ(err, NSAPI_ERROR_TIMEOUT);

    callback_counter = 0;
    test_ctx->attach(ctx_callback);
    cell_callback_data_t cb_data;
    test_ctx->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 2);

    delete test_ctx;

    // Test non blocking
    RIL_CellularDevice *dev2 =  new RIL_CellularDevice(*ril);
    test_context *test_ctx2 = new test_context(*dev2);

    err = test_ctx2->set_blocking(false);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx2->set_sim_ready();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx2->set_sim_ready();
    ASSERT_EQ(err, NSAPI_ERROR_BUSY);

    callback_counter = 0;
    test_ctx2->attach(ctx_callback);
    test_ctx2->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx2->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 2);

    err = test_ctx2->set_sim_ready();
    ASSERT_EQ(err, NSAPI_ERROR_ALREADY);

    delete test_ctx2;
    delete dev2;
}

TEST_F(TestRIL_CellularContext, register_to_network)
{
    test_context *test_ctx = new test_context(*dev);

    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response;
    RILAdaptation_stub::cb_response_len;

    callback_counter = 0;
    test_ctx->attach(ctx_callback);
    cell_callback_data_t cb_data;
    test_ctx->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    // semaphore return 0, so timeout
    CellularDevice_stub::connect_counter = 3;
    Semaphore_stub::acquire_return_value = false;
    nsapi_error_t err = test_ctx->register_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_TIMEOUT);

    // do_initial_attach fails with NSAPI_ERROR_DEVICE_ERROR as there is not valid ril_token
    Semaphore_stub::acquire_return_value = true;
    RILAdaptation_stub::return_null_token = true;
    err = test_ctx->register_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    // do_initial_attach is now success as there is valid ril_token
    RILAdaptation_stub::return_null_token = false;
    err = test_ctx->register_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 2);

    cb_data.status_data = CellularNetwork::RegisteredHomeNetwork;
    test_ctx->cellular_callback((nsapi_event_t)CellularRegistrationStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 3);

    cb_data.status_data = CellularNetwork::Attached;
    test_ctx->cellular_callback((nsapi_event_t)CellularAttachNetwork, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 4);
    delete test_ctx;

    // Test non-blocking
    RIL_CellularDevice *dev2 =  new RIL_CellularDevice(*ril);
    test_context *test_ctx2 = new test_context(*dev2);
    err = test_ctx2->set_blocking(false);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    callback_counter = 0;
    test_ctx2->attach(ctx_callback);
    test_ctx2->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    struct equeue_event ptr;
    equeue_stub.void_ptr = &ptr;
    equeue_stub.call_cb_immediately = false;
    CellularDevice_stub::connect_counter = 0;
    err = test_ctx2->register_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_NO_MEMORY);

    // fail to launch eventqueue
    equeue_stub.call_cb_immediately = true;
    CellularDevice_stub::connect_counter = 0;
    err = test_ctx2->register_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    // start the state ,machine. wait for callbacks
    CellularDevice_stub::connect_counter = 3;
    err = test_ctx2->register_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    callback_counter = 0;
    test_ctx2->attach(ctx_callback);
    test_ctx2->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    // start
    equeue_stub.call_cb_immediately = false;
    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx2->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 3);

    equeue_stub.call_cb_immediately = true;
    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx2->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 5);
    // end
    CellularDevice_stub::connect_counter = 0;
    equeue_stub.call_cb_immediately = true;
    err = test_ctx2->register_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_BUSY);

    cb_data.status_data = CellularNetwork::RegisteredHomeNetwork;
    test_ctx2->cellular_callback((nsapi_event_t)CellularRegistrationStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 6);

    err = test_ctx2->register_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_ALREADY);

    delete test_ctx2;
    delete dev2;
}

TEST_F(TestRIL_CellularContext, attach_to_network)
{
    test_context *test_ctx = new test_context(*dev);

    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response;
    RILAdaptation_stub::cb_response_len;

    callback_counter = 0;
    test_ctx->attach(ctx_callback);
    cell_callback_data_t cb_data;
    test_ctx->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    // semaphore return 0, so timeout
    CellularDevice_stub::connect_counter = 3;
    Semaphore_stub::acquire_return_value = false;
    nsapi_error_t err = test_ctx->attach_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_TIMEOUT);

    // do_initial_attach fails with NSAPI_ERROR_DEVICE_ERROR as there is not valid ril_token
    Semaphore_stub::acquire_return_value = true;
    RILAdaptation_stub::return_null_token = true;
    err = test_ctx->attach_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    // do_initial_attach is now success as there is valid ril_token
    RILAdaptation_stub::return_null_token = false;
    err = test_ctx->attach_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    cb_data.error = NSAPI_ERROR_OK;
    test_ctx->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 2);

    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 3);

    cb_data.status_data = CellularNetwork::RegisteredHomeNetwork;
    test_ctx->cellular_callback((nsapi_event_t)CellularRegistrationStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 4);

    cb_data.status_data = CellularNetwork::Attached;
    test_ctx->cellular_callback((nsapi_event_t)CellularAttachNetwork, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 5);

    delete test_ctx;

    // Test non-blocking
    RIL_CellularDevice *dev2 =  new RIL_CellularDevice(*ril);
    test_context *test_ctx2 = new test_context(*dev2);
    err = test_ctx2->set_blocking(false);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    callback_counter = 0;
    test_ctx2->attach(ctx_callback);
    test_ctx2->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    struct equeue_event ptr;
    equeue_stub.void_ptr = &ptr;
    equeue_stub.call_cb_immediately = false;
    CellularDevice_stub::connect_counter = 0;
    err = test_ctx2->attach_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_NO_MEMORY);

    // fail to launch eventqueue
    equeue_stub.call_cb_immediately = true;
    CellularDevice_stub::connect_counter = 0;
    err = test_ctx2->attach_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    // start the state ,machine. wait for callbacks
    CellularDevice_stub::connect_counter = 3;
    err = test_ctx2->attach_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    callback_counter = 0;
    test_ctx2->attach(ctx_callback);
    test_ctx2->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx2->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 3);

    cb_data.status_data = CellularNetwork::RegisteredHomeNetwork;
    test_ctx2->cellular_callback((nsapi_event_t)CellularRegistrationStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 4);

    CellularDevice_stub::connect_counter = 0;
    equeue_stub.call_cb_immediately = true;
    err = test_ctx2->attach_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_BUSY);

    cb_data.status_data = CellularNetwork::Attached;
    test_ctx2->cellular_callback((nsapi_event_t)CellularAttachNetwork, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 5);

    err = test_ctx2->attach_to_network();
    ASSERT_EQ(err, NSAPI_ERROR_ALREADY);

    delete test_ctx2;
    delete dev2;
}

TEST_F(TestRIL_CellularContext, connect)
{
    test_context *test_ctx = new test_context(*dev);

    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response;
    RILAdaptation_stub::cb_response_len;

    cell_callback_data_t cb_data;
    test_ctx->attach(ctx_callback);
    callback_counter = 0;
    test_ctx->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    // semaphore return 0, so timeout inRIL_CellularContext::check_operation
    CellularDevice_stub::connect_counter = 3;
    Semaphore_stub::acquire_return_value = false;
    nsapi_error_t err = test_ctx->connect();
    ASSERT_EQ(err, NSAPI_ERROR_TIMEOUT);

    nsapi_connection_status_t status = test_ctx->get_connection_status();
    ASSERT_EQ(status, NSAPI_STATUS_DISCONNECTED);

    bool con = test_ctx->is_connected();
    ASSERT_EQ(con, false);

    // do_initial_attach fails with NSAPI_ERROR_DEVICE_ERROR as there is not valid ril_token
    Semaphore_stub::acquire_return_value = true;
    RILAdaptation_stub::return_null_token = true;
    err = test_ctx->connect();
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    // do_initial_attach is now success as there is valid ril_token. Settings call request response is set in ctx_callback is connect_event != 0
    connect_event = 1;
    RILAdaptation_stub::return_null_token = false;
    test_ctx->set_plmn("12345");
    test_ctx->attach(ctx_callback);
    err = test_ctx->connect();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx->connect("1234", "internet", "uname", "pass");
    ASSERT_EQ(err, NSAPI_ERROR_IS_CONNECTED);

    callback_counter = 0;
    test_ctx->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 2);

    cb_data.status_data = CellularNetwork::RegisteredRoaming;
    test_ctx->cellular_callback((nsapi_event_t)CellularRegistrationStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 3);

    cb_data.status_data = CellularNetwork::Attached;
    test_ctx->cellular_callback((nsapi_event_t)CellularAttachNetwork, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 4);

    // test various methods that need to be in connected state and have _onboard_network_stack
    status = test_ctx->get_connection_status();
    ASSERT_EQ(status, NSAPI_STATUS_DISCONNECTED); // return disconnected as using stub

    con = test_ctx->is_connected();
    ASSERT_EQ(con, true);

    char *ifname = (char *)"if1";
    char *ifname2 = test_ctx->get_interface_name(ifname);
    ASSERT_STREQ(ifname, "if1");
    ASSERT_STREQ(ifname2, "if1");

    delete test_ctx;

    // Test non-blocking
    RIL_CellularDevice *dev2 =  new RIL_CellularDevice(*ril);
    test_context *test_ctx2 = new test_context(*dev2);
    err = test_ctx2->set_blocking(false);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    struct equeue_event ptr;
    equeue_stub.void_ptr = &ptr;
    equeue_stub.call_cb_immediately = false;
    CellularDevice_stub::connect_counter = 0;
    err = test_ctx2->connect();
    ASSERT_EQ(err, NSAPI_ERROR_NO_MEMORY);

    callback_counter = 0;
    test_ctx2->attach(ctx_callback);
    test_ctx2->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    // fail to launch eventqueue
    equeue_stub.call_cb_immediately = true;
    CellularDevice_stub::connect_counter = 0;
    err = test_ctx2->connect();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    // start the state ,machine. wait for callbacks
    CellularDevice_stub::connect_counter = 3;
    err = test_ctx2->connect();
    ASSERT_EQ(err, NSAPI_ERROR_IS_CONNECTED);

    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx2->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 3);

    cb_data.status_data = CellularNetwork::RegisteredRoaming;
    test_ctx2->cellular_callback((nsapi_event_t)CellularRegistrationStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 4);

    cb_data.status_data = CellularNetwork::Attached;
    test_ctx2->cellular_callback((nsapi_event_t)CellularAttachNetwork, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 5);

    delete test_ctx2;
    delete dev2;
}

TEST_F(TestRIL_CellularContext, disconnect)
{
    test_context *test_ctx = new test_context(*dev);

    CellularDevice_stub::connect_counter = 3;
    connect_event = 1;
    Semaphore_stub::acquire_return_value = true;
    test_ctx->attach(ctx_callback);

    callback_counter = 0;
    test_ctx->attach(ctx_callback);
    cell_callback_data_t cb_data;
    test_ctx->cellular_callback((nsapi_event_t)CellularDeviceReady, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 1);

    nsapi_error_t err = test_ctx->connect();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    cb_data.status_data = CellularDevice::SimStateReady;
    test_ctx->cellular_callback((nsapi_event_t)CellularSIMStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 3);

    cb_data.status_data = CellularNetwork::RegisteredRoaming;
    test_ctx->cellular_callback((nsapi_event_t)CellularRegistrationStatusChanged, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 4);

    cb_data.status_data = CellularNetwork::Attached;
    test_ctx->cellular_callback((nsapi_event_t)CellularAttachNetwork, (intptr_t)&cb_data);
    ASSERT_EQ(callback_counter, 5);

    err = test_ctx->disconnect();
    ASSERT_EQ(err, NSAPI_ERROR_OK);


    err = test_ctx->connect();
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    err = test_ctx->disconnect();
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    err = test_ctx->connect();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = test_ctx->disconnect();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    delete test_ctx;
}

TEST_F(TestRIL_CellularContext, unsolicited_response)
{
    test_context *test_ctx = new test_context(*dev);
    test_ctx->unsolicited_response(-5, 0, 0);

    test_ctx->unsolicited_response(RIL_UNSOL_DATA_CALL_LIST_CHANGED, 0, 0);

    resp.status             = PDP_FAIL_NETWORK_FAILURE;
    resp.suggestedRetryTime = -1;
    // cid will be assigned below
    resp.active             = 0;
    resp.type               = (char *)IPV4_STR;
    resp.ifname             = (char *)IFNAME_STR;
    resp.addresses          = (char *)"10.0.0.1";
    resp.dnses              = (char *)"8.8.8.8";
    resp.gateways           = (char *)"10.0.0.99";
    resp.pcscf              = (char *)"";
    resp.mtu                = 1500;
    resp.cid                = -1;

    test_ctx->unsolicited_response(RIL_UNSOL_DATA_CALL_LIST_CHANGED, (const void *)&resp, sizeof(resp));

    resp.cid                = 1234;
    test_ctx->unsolicited_response(RIL_UNSOL_DATA_CALL_LIST_CHANGED, (const void *)&resp, sizeof(resp));

    delete test_ctx;
}

TEST_F(TestRIL_CellularContext, enable_hup)
{
    test_context *test_ctx = new test_context(*dev);
    test_ctx->enable_hup(true);
    test_ctx->enable_hup(false);

    delete test_ctx;
}
