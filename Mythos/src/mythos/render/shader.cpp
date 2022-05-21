#include "shader.hpp"
#include "renderer.hpp"

/// MYTodo: Should have the default shaders built into a cpp file, therfore can not be modified or has to be put into the project directory

namespace myl::render {
	constexpr std::string shader_type_to_extenstion(shader_type stage) {
		switch (stage) {
			using enum shader_type;
			case vertex: return ".vert";
			case fragment: return ".frag";
			case compute: return ".comp";
			case geometry: return ".geom";
			case tess_control: return ".tscn";
			case tess_evaluation: return ".tsev";
			case unknown: return ".unkn";
			default: return ".ohno";
		}
	}

	std::shared_ptr<shader> shader::create(const std::filesystem::path& a_file) { /// MYTodo: Should test if shader has already been created
		return renderer::backend()->create_shader(a_file);
	}
}