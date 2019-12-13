
####################
# UNIT TESTS
####################

# Add test specific include paths
set(unittest-includes ${unittest-includes}
  ../features/cellular/framework/RIL
  ../features/cellular/framework/common
  ../features/frameworks/mbed-trace
  ../features/netsocket/cellular
  ../features/netsocket
  ../features/lwipstack
  ../features/lwipstack/lwip-sys
  ../features/lwipstack/lwip
  ../features/lwipstack/lwip/src
  ../features/lwipstack/lwip/src/include
  ../features/lwipstack/lwip/src/include/lwip
)

# Source files
set(unittest-sources
  ../features/cellular/framework/RIL/RIL_CellularContext.cpp
)

# Test files
set(unittest-test-sources
  features/cellular/framework/RIL/ril_cellularcontext/ril_cellularcontexttest.cpp
  stubs/mbed_assert_stub.cpp
  stubs/Mutex_stub.cpp
  stubs/RIL_CellularDevice_stub.cpp
  stubs/ConditionVariable_stub.cpp
  stubs/NetworkInterface_stub.cpp
  stubs/NetworkInterfaceDefaults_stub.cpp
  stubs/CellularDevice_stub.cpp
  stubs/EventQueue_stub.cpp
  stubs/CellularUtil_stub.cpp
  stubs/RILAdaptation_stub.cpp
  stubs/RIL_CellularNetwork_stub.cpp
  stubs/Semaphore_stub.cpp
  stubs/RIL_CellularInformation_stub.cpp
  stubs/FileHandle_stub.cpp
  stubs/CellularContext_stub.cpp
  stubs/LWIPStack_stub.cpp
  stubs/LWIPMemoryManager_stub.cpp
  stubs/NetStackMemoryManager_stub.cpp
  stubs/NetworkStack_stub.cpp
  stubs/equeue_stub.c
  stubs/LWIPInterface_stub.cpp
  stubs/EventFlags_stub.cpp
  stubs/SocketAddress_stub.cpp
  stubs/SocketAddress_stub.cpp
)

set(unittest-test-flags
  -DMBED_CONF_RTOS_PRESENT=1
  -DMBED_CONF_LWIP_IPV4_ENABLED=1
  -DMBED_CONF_LWIP_IP_VER_PREF=4
  -DMBED_CONF_LWIP_MBOX_SIZE=8
  -DMBED_CONF_LWIP_SOCKET_MAX=4
)
