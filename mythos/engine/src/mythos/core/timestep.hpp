#pragma once
#include <myl/definitions.hpp>

namespace mye {
    struct timestep {
    private:
        myl::f64 m_time;
    public:
        MYL_NO_DISCARD constexpr timestep(myl::f64 time = 0.0) noexcept
            : m_time{ time } {}

        MYL_NO_DISCARD constexpr auto ms() const -> myl::f64 { return m_time * 1000.0; }

        constexpr auto operator+=(const timestep& r) -> timestep& { m_time += r; return *this; }

        MYL_NO_DISCARD constexpr operator myl::f64() const noexcept { return m_time; }
    };
}
