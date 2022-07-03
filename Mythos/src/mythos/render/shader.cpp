#include "renderer.hpp"
#include "shader.hpp"

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

	std::shared_ptr<shader> shader::create(const std::filesystem::path& a_file) {
		return renderer::backend()->create_shader(a_file);
	}
}