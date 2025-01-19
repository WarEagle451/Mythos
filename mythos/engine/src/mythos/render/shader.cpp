#include <mythos/assert.hpp>
#include <mythos/render/renderer.hpp>
#include <mythos/render/shader.hpp>

#include <myl/io.hpp>

#include <shaderc/shaderc.hpp>

#include <cctype>

/// MYTodo: Shader
/// - Cache shader_layout and shader types to prevent loading in all the shader code, These are things that will have to be checked if it exists or is up to date
/// - Warn on if more than 1 type of shader stage is present in a file

namespace mye {
    MYL_NO_DISCARD static constexpr auto detect_shader_type(std::string_view type) -> shader_type {
        using enum shader_type;
        if (type == "vertex")
            return vertex;
        else if (type == "fragment" || type == "pixel")
            return fragment;
        else if (type == "compute")
            return compute;
        else if (type == "geometry")
            return geometry;
        else if (type == "tess_control" || "tessellation_control")
            return tessellation_control;
        else if (type == "tess_evaluation" || "tessellation_evaluation")
            return tessellation_evaluation;
        else
            return unknown;
    }

    MYL_NO_DISCARD static constexpr auto shader_data_type_size(shader_data_type type) noexcept -> myl::u32 {
        switch (type) {
            using enum shader_data_type;
            case none:    return 0;
            case boolean: return 1;
            case int1:    return 4;
            case int2:    return 4 * 2;
            case int3:    return 4 * 3;
            case int4:    return 4 * 4;
            case float1:  return 4;
            case float2:  return 4 * 2;
            case float3:  return 4 * 3;
            case float4:  return 4 * 4;
            case mat3:    return 4 * 3 * 3;
            case mat4:    return 4 * 4 * 4;
            default:      return 0;
        }
    }

    MYL_NO_DISCARD static constexpr auto is_shader_data_type(std::string_view view) -> bool {
        if (view == "bool" ||
            view == "int" ||
            view == "ivec2" ||
            view == "ivec3" ||
            view == "ivec4" ||
            view == "float" ||
            view == "vec2" ||
            view == "vec3" ||
            view == "vec4" ||
            view == "mat3" ||
            view == "mat4")
            return true;
        else
            return false;
    }

    MYL_NO_DISCARD static constexpr auto string_to_shader_data_type(std::string_view view) -> shader_data_type {
        using enum shader_data_type;
        if (view == "bool")       return boolean;
        else if (view == "int")   return int1;
        else if (view == "ivec2") return int2;
        else if (view == "ivec3") return int3;
        else if (view == "ivec4") return int4;
        else if (view == "float") return float1;
        else if (view == "vec2")  return float2;
        else if (view == "vec3")  return float3;
        else if (view == "vec4")  return float4;
        else if (view == "mat3")  return mat3;
        else if (view == "mat4")  return mat4;
        else                      return none;
    }

    MYL_NO_DISCARD static constexpr auto shader_type_file_extension(shader_type type) -> const char* {
        switch (type) {
            using enum shader_type;
            case vertex:					return ".vert";
            case fragment:					return ".frag";
            case compute:					return ".comp";
            case geometry:					return ".geom";
            case tessellation_control:		return ".tscn";
            case tessellation_evaluation:	return ".tsev";
            case unknown:					return ".ohno";
            default:                        return ".ohno";
        }
    }

    MYL_NO_DISCARD static auto shader_cache_directory() -> const std::filesystem::path {
        /// MYTODO: This should be obtained from a resource manager
        return std::filesystem::path("cache/shader");
    }

    MYL_NO_DISCARD static auto make_shader_binary_path(const std::filesystem::path& source_path, shader_type type) -> std::filesystem::path {
        // Format: "[cache directory]/[filename].[api].[shader type].spv"
        return shader_cache_directory() / (source_path.stem().string() + render_api_file_extension(renderer::api()) + shader_type_file_extension(type) + ".spv");
    }

    MYL_NO_DISCARD static constexpr auto shader_type_to_shaderc_shader_kind(shader_type type) -> shaderc_shader_kind {
        switch (type) {
            using enum shader_type;
            case vertex:					return shaderc_vertex_shader;
            case fragment:					return shaderc_fragment_shader;
            case compute:					return shaderc_compute_shader;
            case geometry:					return shaderc_geometry_shader;
            case tessellation_control:		return shaderc_tess_control_shader;
            case tessellation_evaluation:	return shaderc_tess_evaluation_shader;
            case unknown:					return shaderc_glsl_infer_from_source;
            default:						return shaderc_glsl_infer_from_source;
        }
    }

    MYL_NO_DISCARD static constexpr auto shaderc_target(render_api api) -> shaderc_target_env {
        switch (api) {
            using enum render_api;
            case none:		return static_cast<shaderc_target_env>(0);
            case vulkan:	return shaderc_target_env_vulkan;
            default:		return static_cast<shaderc_target_env>(0);
        }
    }

    MYL_NO_DISCARD static constexpr auto shaderc_target_version(render_api api) -> shaderc_env_version {
        switch (api) {
            using enum render_api;
            case none:		return static_cast<shaderc_env_version>(0);
            case vulkan:	return shaderc_env_version_vulkan_1_3;
            default:		return static_cast<shaderc_env_version>(0);
        }
    }

    MYL_NO_DISCARD static auto process_shader_source(std::string_view source) -> std::unordered_map<shader_type, std::string> {
        constexpr const char* token = "#type";
        constexpr std::size_t token_length = 5;

        std::unordered_map<shader_type, std::string> sources{};
        std::size_t pos = source.find(token, 0); // Start of first shader type declaration line
        while (pos != std::string::npos) {
            const std::size_t eol = source.find("\r\n", pos); // End of shader type declaration line
            MYTHOS_ASSERT(eol != std::string::npos, "Syntax error");

            const std::size_t begin = pos + token_length + 1; // Start of shader type name (after "#type " keyword)
            const shader_type type = detect_shader_type(source.substr(begin, eol - begin)); // Getting type string and detecting the type
            MYTHOS_ASSERT(type != shader_type::unknown, "Invalid shader type specified");

            const std::size_t next_line_pos = source.find_first_not_of("\r\n", eol); // Beginning of shader code after type declaration
            MYTHOS_ASSERT(next_line_pos != std::string::npos, "Syntax error");
            pos = source.find(token, next_line_pos); // Beginning of next shader type declaration line

            sources[type] = (pos == std::string::npos) ?
                source.substr(next_line_pos) : // Beginning of shader to end of file
                source.substr(next_line_pos, pos - next_line_pos); // Beginning of shader code to line before next shader type declaration line
        }

        return sources;
    }

    MYL_NO_DISCARD constexpr shader_element::shader_element(shader_data_type ptype, std::string_view pname) noexcept
        : type{ ptype }
        , name{ pname }
        , size{ shader_data_type_size(ptype) }
        , offset{ 0 } {}

    MYL_NO_DISCARD constexpr auto shader_element::components() const noexcept -> myl::u32 {
        switch (type) {
            using enum shader_data_type;
            case none:    return 0;
            case boolean: return 1;
            case int1:    return 1;
            case int2:    return 2;
            case int3:    return 3;
            case int4:    return 4;
            case float1:  return 4;
            case float2:  return 4;
            case float3:  return 4;
            case float4:  return 4;
            case mat3:    return 3;
            case mat4:    return 4;
            default:      return 0;
        }
    }

    MYL_NO_DISCARD shader_layout::shader_layout(std::string_view source)
        : m_elements{} {
        constexpr const char* token = "in";

        myl::u32 offset = 0;
        std::string word{};
        for (myl::usize i = 0; i != source.size(); ++i)
            if (std::isspace(source[i]) || source[i] == ';') {
                if (word == token) {
                    ++i; // Move past the last delimiter
                    std::string type{};
                    while (i != source.size() && !(static_cast<bool>(std::isspace(source[i])) || source[i] == ';')) {
                        type += source[i];
                        ++i;
                    }

                    MYTHOS_VERIFY(is_shader_data_type(type), "Unknown shader data type");

                    ++i; // Move past the last delimiter
                    std::string name{};
                    while (i != source.size() && !(static_cast<bool>(std::isspace(source[i])) || source[i] == ';')) {
                        name += source[i];
                        ++i;
                    }

                    shader_element e(string_to_shader_data_type(type), name);
                    e.offset = offset;
                    offset += e.size;
                    m_stride += e.size;
                    m_elements.push_back(std::move(e));
                }

                word.clear();
            }
            else
                word += source[i];
    }

    auto compile_shader_sources(std::unordered_map<shader_type, shader_binary_type>* binaries, const std::unordered_map<shader_type, std::string>& sources, const std::filesystem::path& path) -> void {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        const render_api api = renderer::api();
        options.SetTargetEnvironment(shaderc_target(api), shaderc_target_version(api));
        options.SetOptimizationLevel(shaderc_optimization_level_size);

        for (auto& [type, src] : sources) {
            const std::filesystem::path cache_path = make_shader_binary_path(path, type);

            MYTHOS_TRACE("Compiling Shader '{}'...", cache_path.filename().string());
            const auto result = compiler.CompileGlslToSpv(src.data(), shader_type_to_shaderc_shader_kind(type), path.string().c_str(), options);
            if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
                MYTHOS_FATAL("Stage: {} - {}", shader_type_file_extension(type), result.GetErrorMessage());
                MYTHOS_VERIFY(result.GetCompilationStatus() == shaderc_compilation_status_success, "Failed to compile shader!"); /// MYTODO: Change this to MYTHOS_FAIL_VERIFY
            }
            
            (*binaries)[type] = shader_binary_type(result.begin(), result.end());

            std::filesystem::create_directories(shader_cache_directory()); // Ensure the caching directory exists
            if (std::ofstream out(cache_path, std::ios::binary); out.is_open()) // Caching binary
                out.write(reinterpret_cast<char*>((*binaries)[type].data()), (*binaries)[type].size() * sizeof(shader_binary_type::value_type));
            MYTHOS_TRACE("Success!");
        }
    }

    MYL_NO_DISCARD auto shader::create(const std::filesystem::path& faux_path, std::string_view source, const shader_primitive primitive) -> std::unique_ptr<shader> {
        const std::unordered_map<shader_type, std::string> sources = process_shader_source(source);
        shader_layout layout(sources.at(shader_type::vertex)); /// MYTodo: Currently all shaders must have a vertex module. Therefore this will not fail

        std::unordered_map<shader_type, shader_binary_type> binaries;
        std::unordered_map<shader_type, std::string> uncompiled;
        for (auto& [type, src] : sources) {
            const std::filesystem::path cache_path = make_shader_binary_path(faux_path, type);
            if (std::filesystem::exists(cache_path)) // Binary exists
                binaries[type] = myl::load_binary_into_memory<typename shader_binary_type::value_type>(cache_path);
            else
                uncompiled[type] = std::move(src);
        }

        if (!uncompiled.empty())
            compile_shader_sources(&binaries, uncompiled, faux_path);
        return renderer::backend()->create_shader(binaries, layout, primitive);
    }

    MYL_NO_DISCARD auto shader::create(const std::filesystem::path& path, const shader_primitive primitive) -> std::unique_ptr<shader> {
        const std::unordered_map<shader_type, std::string> sources = process_shader_source(myl::load_into_memory(path));
        shader_layout layout(sources.at(shader_type::vertex)); /// MYTodo: Currently all shaders must have a vertex module. Therefore this will not fail

        std::unordered_map<shader_type, shader_binary_type> binaries;
        std::unordered_map<shader_type, std::string> uncompiled;
        for (auto& [type, src] : sources) {
            const std::filesystem::path cache_path = make_shader_binary_path(path, type);
            const bool compile = !std::filesystem::exists(cache_path) || std::filesystem::last_write_time(path) > std::filesystem::last_write_time(cache_path); // Binary doesn't already exist or isn't up to date

            if (compile)
                uncompiled[type] = std::move(src);
            else
                binaries[type] = myl::load_binary_into_memory<typename shader_binary_type::value_type>(cache_path);
        }

        if (!uncompiled.empty())
            compile_shader_sources(&binaries, uncompiled, path);
        return renderer::backend()->create_shader(binaries, layout, primitive);
    }

    auto shader::destroy(std::unique_ptr<shader>& shader) -> void {
        renderer::backend()->destroy_shader(shader.get());
        shader.reset();
    }
}
