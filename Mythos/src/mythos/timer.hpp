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

	template<typename Clock>
	class timer {
	public:
		using clock = Clock;
	private:
		std::chrono::time_point<clock> m_start;
	public:
		timer()
			: m_start(clock::now()) {}

		void reset() { m_start = clock::now(); }

		template<time_unit TimeUnit = std::chrono::nanoseconds>
		TimeUnit elapsed() const {
			auto time = clock::now() - m_start;
			return std::chrono::duration_cast<TimeUnit>(time);
		}

		template<time_unit TimeUnit = std::chrono::seconds, typename FloatT = f32>
		FloatT elapsed_pretty() const {
			auto time = clock::now() - m_start;
			return std::chrono::duration<FloatT, TimeUnit::period>(time).count();
		}
	};

	using high_resolution_timer = timer<std::chrono::high_resolution_clock>;
}