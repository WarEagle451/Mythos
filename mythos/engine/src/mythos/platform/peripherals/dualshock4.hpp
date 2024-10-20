#pragma once
#include <mythos/platform/hid.hpp>

namespace myth::hid {
    auto dualshock4_controller_data_processing_callback(device* handle, myl::u8* data, myl::u32 byte_count) -> void;

    struct dualshock4_controller : public gamepad, public touchpad {
        MYL_NO_DISCARD dualshock4_controller(id_type id, myl::u16 vendor_id, myl::u16 product_id);
    };
}
