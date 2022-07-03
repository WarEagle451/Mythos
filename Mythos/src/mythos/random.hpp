#pragma once
#include "defines.hpp"

#include <memory>
#include <random>

namespace myl {
	template<class Engine>
	concept random_engine = requires(Engine a_engine, u32 a_u32) {
		a_engine.discard(a_u32);
	};

	template<class Distribution>
	concept distribution = requires(Distribution a_distribution) {
		typename Distribution::result_type;
		a_distribution.reset();
		a_distribution.min();
		a_distribution.max();
	};

	template<random_engine Engine, distribution Distribution>
	class basic_generator {
	public:
		using engine_type = Engine;
		using distribution_type = Distribution;
		using result_type = distribution_type::result_type;
		using seed_type = std::random_device::result_type;
	private:
		distribution_type m_dist;
		std::shared_ptr<engine_type> m_gen;
	public:
		explicit basic_generator(const distribution_type& a_dist = distribution_type())
			: m_gen(std::make_shared<engine_type>(std::random_device()()))
			, m_dist(a_dist) {}
		explicit basic_generator(seed_type a_seed, const distribution_type& a_dist = distribution_type())
			: m_gen(std::make_shared<engine_type>(a_seed))
			, m_dist(a_dist) {}
		explicit basic_generator(std::shared_ptr<engine_type>& a_gen, const distribution_type& a_dist = distribution_type())
			: m_gen(a_gen)
			, m_dist(a_dist) {}

		//@brief Advances the internal state of the generator
		void discard(seed_type a_amount = 1) { m_gen->discard(a_amount); }
		//@brief Calls reset on the distribution
		void reset() { m_dist.reset(); }

		void set_distribution(const distribution_type& a_dist) { m_dist = a_dist; }

		//@return Distribution's lowest possible value
		MYL_NO_DISCARD result_type min() const noexcept { return m_dist.min(); }
		//@return Distribution's highest possible value
		MYL_NO_DISCARD result_type max() const noexcept { return m_dist.max(); }

		MYL_NO_DISCARD result_type operator()() { return m_dist(*m_gen); }
		MYL_NO_DISCARD result_type operator()() const { return m_dist(*m_gen); }
	};

	using generator_bool = basic_generator<std::mt19937_64, std::bernoulli_distribution>;

	using generator_i16 = basic_generator<std::mt19937, std::uniform_int_distribution<i16>>;
	using generator_i32 = basic_generator<std::mt19937, std::uniform_int_distribution<i32>>;
	using generator_i64 = basic_generator<std::mt19937_64, std::uniform_int_distribution<i64>>;

	using generator_u16 = basic_generator<std::mt19937, std::uniform_int_distribution<u16>>;
	using generator_u32 = basic_generator<std::mt19937, std::uniform_int_distribution<u32>>;
	using generator_u64 = basic_generator<std::mt19937_64, std::uniform_int_distribution<u64>>;

	using generator_f32 = basic_generator<std::mt19937_64, std::uniform_real_distribution<f32>>;
	using generator_f64 = basic_generator<std::mt19937_64, std::uniform_real_distribution<f64>>;
}