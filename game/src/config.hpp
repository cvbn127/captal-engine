#ifndef MY_PROJECT_CONFIG_HPP_INCLUDED
#define MY_PROJECT_CONFIG_HPP_INCLUDED

#include <tephra/config.hpp>
#include <captal/config.hpp>

#include <array>

#include <glm/vec4.hpp>

namespace mpr
{

static constexpr tph::version game_version{MY_PROJECT_MAJOR_VERSION, MY_PROJECT_MINOR_VERSION, MY_PROJECT_PATCH_VERSION};

static constexpr std::uint32_t normal_map_binding{3};
static constexpr std::uint32_t height_map_binding{4};
static constexpr std::uint32_t specular_map_binding{5};
static constexpr std::uint32_t emission_map_binding{6};
static constexpr std::uint32_t directional_light_binding{7};
static constexpr std::uint32_t shadow_map_binding{8};

static constexpr std::array<std::uint8_t, 16> dummy_normal_map_data{128, 128, 255, 255, 128, 128, 255, 255, 128, 128, 255, 255, 128, 128, 255, 255};
static constexpr std::array<std::uint8_t, 16> dummy_height_map_data{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static constexpr std::array<std::uint8_t, 16> dummy_specular_map_data{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static constexpr std::array<std::uint8_t, 16> dummy_emission_map_data{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//For texture pools
static constexpr const char* dummy_normal_map_name = "__normal_map";
static constexpr const char* dummy_height_map_name = "__height_map";
static constexpr const char* dummy_specular_map_name = "__specular_map";
static constexpr const char* dummy_emission_map_name = "__emission_map";

struct directional_light
{
    glm::vec4 direction{};
    glm::vec4 ambiant{};
    glm::vec4 diffuse{};
    glm::vec4 specular{};
};

}

#endif
