#pragma once
#include <mythos/defines.hpp>

#include <chrono>
#include <concepts>

namespace myl {
	//@brief Constraints to std::duration specializations or equivalent 
	template<typename Unit>
	concept time_unit = requires(Unit a_unit) {
		typename Unit::period;
		a_unit.count();
	};

	class timer {
	public:
		using clock = std::chrono::high_resolution_clock;
	private:
		std::chrono::time_point<clock> m_start;
	public:
		timer()
			: m_start(clock::now()) {}

		void reset() noexcept { m_start = clock::now(); }

		template<time_unit TimeUnit = std::chrono::nanoseconds>
		u64 elapsed() const {
			auto time = clock::now() - m_start;
			return std::chrono::duration_cast<TimeUnit>(time).count();
		}

		template<time_unit TimeUnit>
		f32 elapsed_pretty() const {
			auto time = clock::now() - m_start;
			return std::chrono::duration<f32, TimeUnit::period>(time).count();
		}
	};
}