//MIT License
//
//Copyright (c) 2021 Alexy Pellegrini
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "render_target.hpp"

#include <cassert>

#include <captal_foundation/stack_allocator.hpp>

#include "vulkan/vulkan_functions.hpp"
#include "vulkan/helper.hpp"

#include "renderer.hpp"
#include "synchronization.hpp"

using namespace tph::vulkan::functions;

namespace tph
{

static vulkan::framebuffer make_framebuffer(renderer& renderer, const render_pass& render_pass, std::span<const std::reference_wrapper<texture_view>> attachments, std::uint32_t width, std::uint32_t height, std::uint32_t layers)
{
    stack_memory_pool<512> pool{};

    auto native_attachments{make_stack_vector<VkImageView>(pool)};
    native_attachments.reserve(std::size(attachments));
    for(const tph::texture_view& attachment : attachments)
    {
        native_attachments.emplace_back(underlying_cast<VkImageView>(attachment));
    }

    return vulkan::framebuffer{underlying_cast<VkDevice>(renderer), underlying_cast<VkRenderPass>(render_pass), native_attachments, VkExtent2D{width, height}, layers};
}

static std::vector<clear_value_t> make_clear_values(std::span<const std::reference_wrapper<texture_view>> attachments)
{
    std::vector<clear_value_t> output{};
    output.resize(std::size(attachments));

    return output;
}

framebuffer::framebuffer(renderer& renderer, const render_pass& render_pass, std::span<const std::reference_wrapper<texture_view>> attachments, std::uint32_t width, std::uint32_t height, std::uint32_t layers)
:m_framebuffer{make_framebuffer(renderer, render_pass, attachments, width, height, layers)}
,m_clear_values{make_clear_values(attachments)}
,m_width{width}
,m_height{height}
,m_layers{layers}
{

}

void framebuffer::set_clear_value(std::uint32_t attachment_index, const clear_value_t& value)
{
    m_clear_values[attachment_index] = value;
}

void framebuffer::set_clear_values(std::vector<clear_value_t> clear_values)
{
    m_clear_values = std::move(clear_values);
}

void set_object_name(renderer& renderer, const framebuffer& object, const std::string& name)
{
    VkDebugUtilsObjectNameInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.objectType = VK_OBJECT_TYPE_FRAMEBUFFER;
    info.objectHandle = reinterpret_cast<std::uint64_t>(underlying_cast<VkFramebuffer>(object));
    info.pObjectName = std::data(name);

    if(auto result{vkSetDebugUtilsObjectNameEXT(underlying_cast<VkDevice>(renderer), &info)}; result != VK_SUCCESS)
        throw vulkan::error{result};
}


//Don't know if I should keep this lul
static constexpr std::size_t stack_size{1024 * 4};
using memory_pool_t = stack_memory_pool<stack_size>;
template<typename T>
using stack_vector_t = stack_vector<T, stack_size>;

static stack_vector_t<VkAttachmentDescription> make_attachments(memory_pool_t& pool, std::span<const attachment_description> attachments)
{
    auto output{make_stack_vector<VkAttachmentDescription>(pool)};
    output.reserve(std::size(attachments));

    for(auto&& attachment : attachments)
    {
        auto& native_attachment{output.emplace_back()};

        native_attachment.format = static_cast<VkFormat>(attachment.format);
        native_attachment.samples = static_cast<VkSampleCountFlagBits>(attachment.sample_count);
        native_attachment.loadOp = static_cast<VkAttachmentLoadOp>(attachment.load_op);
        native_attachment.storeOp = static_cast<VkAttachmentStoreOp>(attachment.store_op);
        native_attachment.stencilLoadOp = static_cast<VkAttachmentLoadOp>(attachment.stencil_load_op);
        native_attachment.stencilStoreOp = static_cast<VkAttachmentStoreOp>(attachment.stencil_store_op);
        native_attachment.initialLayout = static_cast<VkImageLayout>(attachment.initial_layout);
        native_attachment.finalLayout = static_cast<VkImageLayout>(attachment.final_layout);
    }

    return output;
}

static VkAttachmentReference convert_attachment(const attachment_reference& attachment)
{
    VkAttachmentReference output{};

    output.layout = static_cast<VkImageLayout>(attachment.layout);
    output.attachment = attachment.attachment;

    return output;
}

static stack_vector_t<VkAttachmentReference> convert_attachments(memory_pool_t& pool, std::span<const attachment_reference> attachments)
{
    auto output{make_stack_vector<VkAttachmentReference>(pool)};
    output.reserve(std::size(attachments));

    for(auto&& attachment : attachments)
    {
        output.emplace_back(convert_attachment(attachment));
    }

    return output;
}

struct subpass_data
{
    stack_vector_t<VkAttachmentReference> input_attachments{};
    stack_vector_t<VkAttachmentReference> color_attachments{};
    stack_vector_t<VkAttachmentReference> resolve_attachments{};
    std::optional<VkAttachmentReference> depth_attachment{};
    const std::vector<std::uint32_t>* preserve_attachments{};
};

static stack_vector_t<subpass_data> make_subpasses_data(memory_pool_t& pool, std::span<const subpass_description> subpasses)
{
    auto output{make_stack_vector<subpass_data>(pool)};
    output.reserve(std::size(subpasses));

    for(auto&& subpass : subpasses)
    {
        auto& native_subpass{output.emplace_back()};

        native_subpass.input_attachments = convert_attachments(pool, subpass.input_attachments);
        native_subpass.color_attachments = convert_attachments(pool, subpass.color_attachments);
        native_subpass.resolve_attachments = convert_attachments(pool, subpass.resolve_attachments);

        if(subpass.depth_attachment.has_value())
        {
            native_subpass.depth_attachment = convert_attachment(*subpass.depth_attachment);
        }

        native_subpass.preserve_attachments = &subpass.preserve_attachments;
    }

    return output;
}

static stack_vector_t<VkSubpassDescription> make_subpasses(memory_pool_t& pool, std::span<const subpass_data> subpasses_data)
{
    auto output{make_stack_vector<VkSubpassDescription>(pool)};
    output.reserve(std::size(subpasses_data));

    for(auto&& data : subpasses_data)
    {
        auto& native_subpass{output.emplace_back()};

        native_subpass.inputAttachmentCount = static_cast<std::uint32_t>(std::size(data.input_attachments));
        native_subpass.pInputAttachments = std::data(data.input_attachments);
        native_subpass.colorAttachmentCount = static_cast<std::uint32_t>(std::size(data.color_attachments));
        native_subpass.pColorAttachments = std::data(data.color_attachments);
        native_subpass.pResolveAttachments = std::data(data.resolve_attachments);
        native_subpass.preserveAttachmentCount = static_cast<std::uint32_t>(std::size(*data.preserve_attachments));
        native_subpass.pPreserveAttachments = std::data(*data.preserve_attachments);

        if(data.depth_attachment.has_value())
        {
            native_subpass.pDepthStencilAttachment = &(*data.depth_attachment);
        }
    }

    return output;
}

static stack_vector_t<VkSubpassDependency> make_dependencies(memory_pool_t& pool, std::span<const subpass_dependency> dependencies)
{
    auto output{make_stack_vector<VkSubpassDependency>(pool)};
    output.reserve(std::size(dependencies));

    for(auto&& dependency : dependencies)
    {
        auto& native_dependency{output.emplace_back()};

        native_dependency.srcSubpass = dependency.source_subpass;
        native_dependency.dstSubpass = dependency.destination_subpass;
        native_dependency.srcStageMask = static_cast<VkPipelineStageFlags>(dependency.source_stage);
        native_dependency.dstStageMask = static_cast<VkPipelineStageFlags>(dependency.destination_stage);
        native_dependency.srcAccessMask = static_cast<VkAccessFlags>(dependency.source_access);
        native_dependency.dstAccessMask = static_cast<VkAccessFlags>(dependency.destination_access);
        native_dependency.dependencyFlags = static_cast<VkDependencyFlags>(dependency.dependency_flags);
    }

    return output;
}

render_pass::render_pass(renderer& renderer, const render_pass_info& info)
{
    memory_pool_t pool{};

    const auto attachments{make_attachments(pool, info.attachments)};
    const auto subpasses_data{make_subpasses_data(pool, info.subpasses)};
    const auto subpass{make_subpasses(pool, subpasses_data)};
    const auto dependencies{make_dependencies(pool, info.dependencies)};

    m_render_pass = vulkan::render_pass{underlying_cast<VkDevice>(renderer), attachments, subpass, dependencies};
}

void set_object_name(renderer& renderer, const render_pass& object, const std::string& name)
{
    VkDebugUtilsObjectNameInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.objectType = VK_OBJECT_TYPE_RENDER_PASS;
    info.objectHandle = reinterpret_cast<std::uint64_t>(underlying_cast<VkRenderPass>(object));
    info.pObjectName = std::data(name);

    if(auto result{vkSetDebugUtilsObjectNameEXT(underlying_cast<VkDevice>(renderer), &info)}; result != VK_SUCCESS)
        throw vulkan::error{result};
}

}
