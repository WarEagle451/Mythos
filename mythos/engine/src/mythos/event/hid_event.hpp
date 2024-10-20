#pragma once
#include <mythos/platform/hid.hpp>
#include <mythos/event/event.hpp>

namespace myth::event {
    class hid_button_pressed : public base {
        hid::device* m_device;
        hid_button_code m_code;
    public:
        MYL_NO_DISCARD constexpr hid_button_pressed(hid::device* device, hid_button_code code)
            : m_device{ device }, m_code{ code } {}

        MYL_NO_DISCARD constexpr auto device() const noexcept -> hid::device* { return m_device; }
        MYL_NO_DISCARD constexpr auto buttons() const noexcept -> hid_button_code { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::hid | category_flags::input | category_flags::hid_button)
            MYTHOS_IMPL_EVENT_TYPE(hid_button_pressed)
    };

    class hid_button_released : public base {
        hid::device* m_device;
        hid_button_code m_code;
    public:
        MYL_NO_DISCARD constexpr hid_button_released(hid::device* device, hid_button_code code)
            : m_device{ device }, m_code{ code } {}

        MYL_NO_DISCARD constexpr auto device() const noexcept -> hid::device* { return m_device; }
        MYL_NO_DISCARD constexpr auto buttons() const noexcept -> hid_button_code { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::hid | category_flags::input | category_flags::hid_button)
        MYTHOS_IMPL_EVENT_TYPE(hid_button_released)
    };

    class hid_added : public base {
        const hid::device::id_type m_id;
        const myl::u16     m_vendor;
        const myl::u16     m_product;
    public:
        MYL_NO_DISCARD constexpr hid_added(hid::device::id_type id, myl::u16 vendor, myl::u16 product)
            : m_id{ id }
            , m_vendor{ vendor }
            , m_product{ product } {}

        MYL_NO_DISCARD constexpr auto id() const noexcept -> hid::device::id_type { return m_id; }
        MYL_NO_DISCARD constexpr auto vendor() const noexcept -> myl::u16 { return m_vendor; }
        MYL_NO_DISCARD constexpr auto product() const noexcept -> myl::u16 { return m_product; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::hid)
        MYTHOS_IMPL_EVENT_TYPE(hid_added)
    };

    class hid_removed : public base {
        const hid::device::id_type m_id;
    public:
        MYL_NO_DISCARD constexpr hid_removed(hid::device::id_type id)
            : m_id{ id } {}

        MYL_NO_DISCARD constexpr auto id() const noexcept -> hid::device::id_type { return m_id; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::hid)
        MYTHOS_IMPL_EVENT_TYPE(hid_removed)
    };
}
