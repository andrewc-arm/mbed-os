
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
)

# Source files
set(unittest-sources
  ../features/cellular/framework/RIL/RILAdaptation.cpp
)

# Test files
set(unittest-test-sources
  features/cellular/framework/RIL/riladaptation/riladaptationtest.cpp
  stubs/mbed_assert_stub.cpp
  stubs/Mutex_stub.cpp
  stubs/CellularContext_stub.cpp
  stubs/ConditionVariable_stub.cpp
  stubs/NetworkInterface_stub.cpp
  stubs/NetworkInterfaceDefaults_stub.cpp
  stubs/RIL_CellularDevice_stub.cpp
  stubs/CellularDevice_stub.cpp
  stubs/EventQueue_stub.cpp
  stubs/RIL_stub.cpp
  stubs/RIL_CellularNetwork_stub.cpp
  stubs/Semaphore_stub.cpp
  stubs/FileHandle_stub.cpp
)

set(unittest-test-flags
  -DMBED_CONF_RTOS_PRESENT=1
)