#pragma once
#include <mythos/input/device.hpp>
#include <mythos/event/event.hpp>

namespace myth::event {
    class device_button_pressed : public base {
        device* m_device;
        button_code m_code;
    public:
        MYL_NO_DISCARD constexpr device_button_pressed(device* device, button_code code)
            : m_device{ device }, m_code{ code } {}

        MYL_NO_DISCARD constexpr auto device() const noexcept -> device* { return m_device; }
        MYL_NO_DISCARD constexpr auto buttons() const noexcept -> button_code { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::device | category_flags::input | category_flags::device_button)
            MYTHOS_IMPL_EVENT_TYPE(device_button_pressed)
    };

    class device_button_released : public base {
        device* m_device;
        button_code m_code;
    public:
        MYL_NO_DISCARD constexpr device_button_released(device* device, button_code code)
            : m_device{ device }, m_code{ code } {}

        MYL_NO_DISCARD constexpr auto device() const noexcept -> device* { return m_device; }
        MYL_NO_DISCARD constexpr auto buttons() const noexcept -> button_code { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::device | category_flags::input | category_flags::device_button)
        MYTHOS_IMPL_EVENT_TYPE(device_button_released)
    };

    class device_added : public base {
        const native_device_handle_type m_id;
        const myl::u16                  m_vendor;
        const myl::u16                  m_product;
    public:
        MYL_NO_DISCARD constexpr device_added(native_device_handle_type id, myl::u16 vendor, myl::u16 product)
            : m_id{ id }
            , m_vendor{ vendor }
            , m_product{ product } {}

        MYL_NO_DISCARD constexpr auto id() const noexcept -> native_device_handle_type { return m_id; }
        MYL_NO_DISCARD constexpr auto vendor() const noexcept -> myl::u16 { return m_vendor; }
        MYL_NO_DISCARD constexpr auto product() const noexcept -> myl::u16 { return m_product; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::device)
        MYTHOS_IMPL_EVENT_TYPE(device_added)
    };

    class device_removed : public base {
        const native_device_handle_type m_id;
    public:
        MYL_NO_DISCARD constexpr device_removed(native_device_handle_type id)
            : m_id{ id } {}

        MYL_NO_DISCARD constexpr auto id() const noexcept -> native_device_handle_type { return m_id; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::device)
        MYTHOS_IMPL_EVENT_TYPE(device_removed)
    };
}
