#pragma once
#include <mythos/defines.hpp>

#include "timestep.hpp"

#include <string>

namespace myl {
	class layer {
	protected:
		std::string m_name;
	public:
		MYL_API layer(std::string_view a_name)
			: m_name(a_name) {}

		MYL_API virtual ~layer() = default;

		MYL_API virtual void on_attach() = 0;
		MYL_API virtual void on_detach() = 0;
		MYL_API virtual void update(timestep) = 0;
		MYL_API virtual void render() = 0;
		/// MYTodo: on event, aka on_resize and stuff like that

		MYL_API MYL_NO_DISCARD std::string_view name() const { return m_name; }
	};
}