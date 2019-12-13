#include "myCellularDevice.h"
#include "CellularDevice_stub.h"

using namespace mbed;

MBED_WEAK CellularDevice *CellularDevice::get_default_instance()
{
    if (CellularDevice_stub::create_in_get_default) {
        static myCellularDevice dev(NULL);
        return &dev;
    } else {
        return NULL;
    }
}
