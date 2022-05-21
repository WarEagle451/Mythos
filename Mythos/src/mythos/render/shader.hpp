#pragma once
#include <mythos/defines.hpp>

#include <memory>
#include <filesystem>
#include <string>

namespace myl::render {
	enum class shader_type {
		vertex,
		fragment,
		compute,
		geometry,
		tess_control,
		tess_evaluation,
		unknown
	};

	/// MYTodo: Why does this need MYL_API?
	MYL_API MYL_NO_DISCARD constexpr std::string shader_type_to_extenstion(shader_type);

	class shader {
	public:
		/// MYTodo: create from file or provide the src code
		/// MYTodo: Find a way to call this before backend is assigned
		MYL_API MYL_NO_DISCARD static std::shared_ptr<shader> create(const std::filesystem::path&);

		MYL_API virtual ~shader() = default;

		MYL_API virtual void bind() const = 0;
		MYL_API virtual void unbind() const = 0;
	};
}
