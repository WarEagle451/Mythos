#pragma once
#include <mythos/core/layer.hpp>

namespace tb {
	class testbed_layer : public myl::layer {
	public:
		testbed_layer();
		~testbed_layer();

		void on_attach() override;
		void on_detach() override;
		void update(myl::timestep) override;
		void render() override;
	};
}