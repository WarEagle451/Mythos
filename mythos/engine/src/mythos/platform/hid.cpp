#include <mythos/input.hpp>
#include <mythos/log.hpp>
#include <mythos/platform/hid.hpp>
#include <mythos/platform/peripherals/dualsense.hpp>
#include <mythos/platform/peripherals/dualshock4.hpp>

namespace myth::hid {
    static auto deduce_hid_data_processing_callback(myl::u16 vendor_id, myl::u16 product_id) -> device::data_processing_callback {
        switch (vendor_id) {
            case 0x54C: // Sony
                switch (product_id) {
                    case 0x05C4: MYL_FALLTHROUGH; // Dualshock 4
                    case 0x09CC: return dualshock4_controller_data_processing_callback;
                    case 0x0CE6: return dualsense_controller_data_processing_callback;
                    default: break;
                }
                break;
            default: break;
        }

        MYTHOS_ERROR("Device HID processing callback can't be determined. Vendor ID: {}, Product ID: {}", vendor_id, product_id);        
        return nullptr;
    }

    MYL_NO_DISCARD device::device(id_type id, myl::u16 vendor_id, myl::u16 product_id, data_processing_callback callback)
        : processing_callback{ callback == nullptr ? deduce_hid_data_processing_callback(vendor_id, product_id) : callback }
        , id{ id }
        , vendor_id{ vendor_id }
        , product_id{ product_id }
        , features{} {

    }

    MYL_NO_DISCARD gamepad::gamepad(id_type id, myl::u16 vendor_id, myl::u16 product_id, data_processing_callback callback)
        : device(id, vendor_id, product_id, callback)
        , buttons{} {

    }

    auto create(device::id_type id, myl::u16 vendor_id, myl::u16 product_id) -> std::unique_ptr<device> {
        switch (vendor_id) {
            case 0x054C: // Sony
                switch (product_id) {
                    case 0x05C4: MYL_FALLTHROUGH;                                                           // Dualshock 4
                    case 0x09CC: return std::make_unique<dualshock4_controller>(id, vendor_id, product_id); // Dualshock 4
                    case 0x0CE6: return std::make_unique<dualsense_controller>(id, vendor_id, product_id);  // DualSense
                    ///case 0x0DF2:  // Dualsense Edge 
                    default:
                        MYTHOS_ERROR("Unknown Sony device detected, product ID: {}", product_id);
                        break;
                } break;
            default:
                MYTHOS_ERROR("Unknown device detected, vendor ID: {}, product ID: {}", vendor_id, product_id);
                break;
        }

        return nullptr;
    }
}
