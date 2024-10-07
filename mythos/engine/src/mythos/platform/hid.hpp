#pragma once
#include <mythos/core/hid_button_code.hpp>

#include <myl/math/vec2.hpp>

#include <memory>

namespace myth {
    /// MYTODO: Improve HID, so that upcasting is not a trusted thing

    struct hid {
        using id_type = myl::usize;
        using processing_callback = void(*)(hid* device, myl::u8* data, myl::u32 byte_count);

        MYL_NO_DISCARD hid(id_type id, myl::u16 vendor_id, myl::u16 product_id, processing_callback callback = nullptr);
        
        processing_callback process_callback;
        const id_type       id;
        const myl::u16      vendor;
        const myl::u16      product;

        hid_button_code button_states;

        virtual auto update() -> void = 0;
    };

    struct gamepad : public hid {
        MYL_NO_DISCARD gamepad(id_type id, myl::u16 vendor_id, myl::u16 product_id, processing_callback callback = nullptr);
        
        myl::f32vec2 left_stick{ 0.f, 0.f };
        myl::f32vec2 right_stick{ 0.f, 0.f };
        myl::f32vec2 stick_deadzones{ .1f, .1f };
        myl::f32vec2 trigger_deltas{ 0.f, 0.f };

        auto update() -> void override;
    };

    auto deduce_and_create_hid(hid::id_type id, myl::u16 vendor, myl::u16 product) -> std::unique_ptr<hid>;
}
