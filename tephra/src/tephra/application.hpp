#ifndef TEPHRA_APPLICATION_HPP_INCLUDED
#define TEPHRA_APPLICATION_HPP_INCLUDED

#include "config.hpp"

#include <limits>
#include <span>
#include <functional>

#include "vulkan/vulkan.hpp"
#include "vulkan/memory.hpp"

#include "hardware.hpp"

namespace tph
{

TEPHRA_API tph::version enumerate_instance_version();

enum class application_options : std::uint32_t
{
    none = 0x00,
    enable_validation = 0x01,
    enable_verbose_validation = 0x02,
};

class TEPHRA_API application
{
    template<typename VulkanObject, typename... Args>
    friend VulkanObject underlying_cast(const Args&...) noexcept;

public:
    using filter_type = std::function<bool(const physical_device&)>;
    using comparator_type = std::function<bool(const physical_device&, const physical_device&)>;

public:
    constexpr application() = default;

    explicit application(const std::string& application_name, version application_version, application_options options = application_options::none);
    explicit application(const std::string& application_name, version application_version, version api_version, application_options options = application_options::none);
    explicit application(vulkan::instance instance, tph::version api_version, vulkan::debug_messenger debug_messenger = vulkan::debug_messenger{});

    ~application() = default;
    application(const application&) = delete;
    application& operator=(const application&) = delete;
    application(application&& other) noexcept = default;
    application& operator=(application&& other) noexcept = default;

    const physical_device& select_physical_device(const filter_type& required, const comparator_type& comparator = comparator_type{}) const;
    const physical_device& default_physical_device() const;

    template<typename... Surfaces>
    const physical_device& default_physical_device(const Surfaces&... surfaces) const
    {
        static_assert((std::is_same_v<tph::surface, std::decay_t<Surfaces>> && ...), "One of the parameters is not a tph::surface.");

        const auto requirements = [&surfaces...](const physical_device& device) -> bool
        {
            return (... && device.support_presentation(surfaces));
        };

        return select_physical_device(requirements, default_physical_device_comparator);
    }

    application_options options() const noexcept
    {
        return m_options;
    }

    tph::version api_version() const noexcept
    {
        return m_version;
    }

    std::span<const physical_device> enumerate_physical_devices() const noexcept
    {
        return m_physical_devices;
    }

private:
    application_options m_options{};
    vulkan::instance m_instance{};
    vulkan::debug_messenger m_debug_messenger{};
    tph::version m_version{};
    std::vector<physical_device> m_physical_devices{};
};

template<>
inline VkInstance underlying_cast(const application& initializer) noexcept
{
    return initializer.m_instance;
}

}

template<> struct tph::enable_enum_operations<tph::application_options> {static constexpr bool value{true};};


#endif
