#pragma once
#include "timestep.hpp"

#include <mythos/defines.hpp>
#include <mythos/event/event.hpp>

#include <string>

namespace myl { /// MYTodo: All layers should have a way to block events
	class layer {
	protected:
		std::string m_name;
	public:
		MYL_API layer(std::string_view a_name)
			: m_name(a_name) {}

		MYL_API virtual ~layer() = default;

		MYL_API virtual void on_attach() = 0;
		MYL_API virtual void on_detach() = 0;
		MYL_API virtual void on_event(myl::event&) = 0;
		MYL_API virtual void update(timestep) = 0;
		MYL_API virtual void render() = 0;

		MYL_API MYL_NO_DISCARD std::string_view name() const { return m_name; }
	};
}