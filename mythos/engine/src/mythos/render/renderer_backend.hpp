#pragma once
#include <mythos/render/render_buffer.hpp>
#include <mythos/render/shader.hpp>

#include <myl/math/vec2.hpp>
#include <myl/math/vec3.hpp>

#include <memory>

namespace mye {
    enum class render_api {
        vulkan,
        none
    };

    MYL_NO_DISCARD static constexpr auto render_api_to_string(render_api api) -> const char* {
        switch (api) {
            using enum render_api;
            case vulkan: return "Vulkan";
            case none:   return "None";
            default:     return "Unknown";
        }
    }

    MYL_NO_DISCARD static constexpr auto render_api_file_extension(render_api api) -> const char* {
        switch (api) {
            using enum render_api;
            case vulkan: return ".vulk";
            case none:   return ".ohno";
            default:     return ".ohno";
        }
    }

    struct renderer_configuration {
        myl::f32vec3 clear_color = myl::f32vec3(0.f);
        bool vsync = true;
    };

    struct frame_data {
        bool good;

        myl::u32 index;
    };

    struct indexed_draw_data {
        shader&        shader;
        render_buffer& vertex_buffer;
        render_buffer& index_buffer;
        myl::u32       index_count = 0;
    };

    class renderer_backend {
    public:
        MYL_NO_DISCARD static auto create(render_api api, const renderer_configuration& config) -> std::unique_ptr<renderer_backend>;

        virtual ~renderer_backend() = default;

        MYL_NO_DISCARD virtual auto create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, const shader_layout& layout, shader_primitive primitive) -> std::unique_ptr<shader> = 0;
        virtual auto destroy_shader(shader* shader) -> void = 0;

        MYL_NO_DISCARD virtual auto create_render_buffer(render_buffer_usage usage, myl::usize bytes) -> std::unique_ptr<render_buffer> = 0;
        virtual auto destroy_render_buffer(render_buffer* buffer) -> void = 0;

        virtual auto set_clear_color(const myl::f32vec3& color) -> void = 0;
        virtual auto set_vsync(bool value) -> void = 0;

        virtual auto begin_frame(frame_data* frame_data) -> void = 0;
        virtual auto end_frame() -> void = 0;

        virtual auto draw(indexed_draw_data& draw_data) -> void = 0;

        virtual auto prepare_shutdown() -> void = 0;

        virtual auto on_window_resize(const myl::i32vec2& dimensions) -> void = 0;
    };
}
