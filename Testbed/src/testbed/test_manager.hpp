#pragma once
#include <mythos/core/log.hpp> /// MYTodo: Add Testbed logger
#include <mythos/timer.hpp>

#include <functional>
#include <vector>
#include <string>

namespace tb {
	struct test_entry {
		std::string name;
		std::function<bool()> test;
	};

	class test_manager {
		std::vector<test_entry> m_tests;
	public:
		test_manager();

		void add(const char* a_name, const std::function<bool()>& a_func) { m_tests.emplace_back(a_name, a_func); }

		void run() const {
			u32 passed = 0;
			u32 failed = 0;

			for (auto& t : m_tests) {
				myl::high_resolution_timer timer;
				bool result = t.test();
				auto time = timer.elapsed_pretty<std::chrono::milliseconds>();

				if (result) {
					++passed;
					MYL_CORE_INFO("Passed: '{}', time {}ms", t.name, time);
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