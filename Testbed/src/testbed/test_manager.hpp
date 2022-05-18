#pragma once
#include <mythos/core/log.hpp>
#include <mythos/timer.hpp>

#include <functional>
#include <vector>
#include <string>

namespace tb {
	struct test_entry {
		std::string name;
		std::function<bool()> test;
		u64 max_time;
	};

	class test_manager {
		std::vector<test_entry> m_tests;
	public:
		test_manager();

		void add(const char* a_name, const std::function<bool()>& a_func, u64 a_max_time = ~0) { m_tests.emplace_back(a_name, a_func, a_max_time); }

		void run() const {
			u32 passed = 0;
			u32 failed = 0;

			for (auto& t : m_tests) {
				myl::high_resolution_timer timer;
				bool result = t.test();
				auto time = timer.elapsed().count();

				if (result) {
					++passed;
					static_cast<u64>(time) > t.max_time ?
						MYL_CORE_WARN("Passed: '{}', {}ns exceeds max time of {}ns", t.name, time, t.max_time) :
						MYL_CORE_INFO("Passed: '{}', time {}ns", t.name, time);
				}
				else {
					++failed;
					MYL_CORE_ERROR("Failed: '{}'", t.name);
				}
			}

			MYL_CORE_INFO("Results: {} passed, {} failed", passed, failed);
		}
	};
}