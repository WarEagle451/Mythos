#pragma once
#include <myl/definitions.hpp>

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace mye {
    enum class shader_primitive {
        point,
        line,
        triangle
    };

    enum class shader_type {
        vertex,
        fragment,
        compute,
        geometry,
        tessellation_control,
        tessellation_evaluation,

        unknown
    };

    enum class shader_data_type {
        none,
        boolean,
        int1,
        int2,
        int3,
        int4,
        float1,
        float2,
        float3,
        float4,
        mat3,
        mat4,
    };

    using shader_binary_type = std::vector<myl::u32>;

    struct shader_element {
        shader_data_type type = shader_data_type::none;
        std::string name      = "";
        myl::u32 size         = 0;
        myl::u32 offset       = 0;

        MYL_NO_DISCARD constexpr shader_element(shader_data_type type, std::string_view name) noexcept;

        MYL_NO_DISCARD constexpr auto components() const noexcept -> myl::u32;
    };

    class shader_layout {
        std::vector<shader_element> m_elements;
        myl::u32 m_stride = 0;
    public:
        MYL_NO_DISCARD shader_layout(std::string_view source);

        MYL_NO_DISCARD constexpr auto elements() const noexcept -> const std::vector<shader_element>& { return m_elements; }
        MYL_NO_DISCARD constexpr auto stride() const noexcept -> myl::u32 { return m_stride; }

        MYL_NO_DISCARD constexpr auto begin() { return m_elements.begin(); }
        MYL_NO_DISCARD constexpr auto begin() const { return m_elements.cbegin(); }
        MYL_NO_DISCARD constexpr auto end() { return m_elements.end(); }
        MYL_NO_DISCARD constexpr auto end() const { return m_elements.cend(); }
    };

    class shader {
    public:
        /// MYTodo: creates should probs share to a shader lib like in legacy mythos

        MYL_NO_DISCARD static auto create(const std::filesystem::path& faux_path, std::string_view source, const shader_primitive primitive) -> std::unique_ptr<shader>;
        MYL_NO_DISCARD static auto create(const std::filesystem::path& path, const shader_primitive primitive) -> std::unique_ptr<shader>;
        static auto destroy(std::unique_ptr<shader>& shader) -> void;

        virtual ~shader() = default;
    };
}
