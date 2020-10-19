#include "render_technique.hpp"

#include "engine.hpp"
#include "vertex.hpp"

#include <iostream>

namespace cpt
{

descriptor_set::descriptor_set(descriptor_pool& parent, tph::descriptor_set set) noexcept
:m_parent{&parent}
,m_set{std::move(set)}
{

}


descriptor_pool::descriptor_pool(render_technique& parent, tph::descriptor_pool pool)
:m_parent{&parent}
,m_pool{std::move(pool)}
{
    for(auto&& set : m_sets)
    {
        set = std::make_shared<descriptor_set>(*this, tph::descriptor_set{engine::instance().renderer(), m_pool, m_parent->descriptor_set_layout()});
    }
}

descriptor_set_ptr descriptor_pool::allocate() noexcept
{
    for(auto&& set : m_sets)
    {
        if(set.use_count() == 1)
        {
            return set;
        }
    }

    return nullptr;
}

bool descriptor_pool::unused() const noexcept
{
    return std::all_of(std::begin(m_sets), std::end(m_sets), [](const descriptor_set_ptr& data){return data.use_count() == 1;});
}

#ifdef CAPTAL_DEBUG
void descriptor_pool::set_name(std::string_view name)
{
    const std::string name_str{name};

    tph::set_object_name(engine::instance().renderer(), m_pool, name_str);

    for(std::size_t i{}; i < std::size(m_sets); ++i)
    {
        tph::set_object_name(engine::instance().renderer(), m_pool, name_str + " descriptor set #" + std::to_string(i));
    }
}
#endif

static std::vector<tph::descriptor_set_layout_binding> make_bindings(const std::vector<tph::descriptor_set_layout_binding>& info)
{
    std::vector<tph::descriptor_set_layout_binding> output{};

    //0: view uniform
    //1: model uniform
    //2: texture sampler
    output.emplace_back(tph::shader_stage::vertex, 0, tph::descriptor_type::uniform_buffer);
    output.emplace_back(tph::shader_stage::vertex, 1, tph::descriptor_type::uniform_buffer);
    output.emplace_back(tph::shader_stage::fragment, 2, tph::descriptor_type::image_sampler);

    output.insert(std::end(output), std::begin(info), std::end(info));

    return output;
}

static tph::graphics_pipeline_info make_info(const render_technique_info& info)
{
    tph::graphics_pipeline_info output{};

    if(std::empty(info.color_blend.attachments))
    {
        output.color_blend.attachments.emplace_back();
    }
    else
    {
        output.color_blend = info.color_blend;
    }

    if(std::empty(info.stages))
    {
        output.stages.emplace_back(engine::instance().default_vertex_shader());
        output.stages.emplace_back(engine::instance().default_fragment_shader());
    }
    else
    {
        for(auto&& stage : info.stages)
        {
            output.stages.emplace_back(stage.shader, stage.name, stage.specialisation_info);
        }
    }

    output.vertex_input.bindings.emplace_back(0, sizeof(vertex));
    output.vertex_input.attributes.emplace_back(0, 0, tph::vertex_format::vec3f, offsetof(vertex, position));
    output.vertex_input.attributes.emplace_back(1, 0, tph::vertex_format::vec4f, offsetof(vertex, color));
    output.vertex_input.attributes.emplace_back(2, 0, tph::vertex_format::vec2f, offsetof(vertex, texture_coord));
    output.tesselation = info.tesselation;
    output.viewport.viewport_count = 1;
    output.rasterization = info.rasterization;
    output.multisample = info.multisample;
    output.depth_stencil = info.depth_stencil;
    output.dynamic_states.emplace_back(tph::dynamic_state::viewport);
    output.dynamic_states.emplace_back(tph::dynamic_state::scissor);

    return output;
}

render_technique::render_technique(const render_target_ptr& target, const render_technique_info& info)
:m_bindings{make_bindings(info.stages_bindings)}
,m_ranges{info.push_constant_ranges}
,m_descriptor_set_layout{engine::instance().renderer(), m_bindings}
,m_layout{engine::instance().renderer(), std::array{std::ref(m_descriptor_set_layout)}, m_ranges}
,m_pipeline{engine::instance().renderer(), target->get_render_pass(), make_info(info), m_layout}
{
    m_sizes.reserve(std::size(m_bindings));
    for(auto&& binding : m_bindings)
    {
        m_sizes.emplace_back(tph::descriptor_pool_size{binding.type, static_cast<std::uint32_t>(binding.count * descriptor_pool::pool_size)});
    }
}

descriptor_set_ptr render_technique::make_set()
{
    std::lock_guard lock{m_mutex};

    for(auto&& pool : m_pools)
    {
        auto set{pool->allocate()};

        if(set)
        {
            return set;
        }
    }

    m_pools.emplace_back(std::make_unique<descriptor_pool>(*this, tph::descriptor_pool{engine::instance().renderer(), m_sizes, static_cast<std::uint32_t>(descriptor_pool::pool_size)}));

#ifdef CAPTAL_DEBUG
    if(!std::empty(m_name))
    {
        m_pools.back()->set_name(m_name + " descriptor pool #" + std::to_string(std::size(m_pools) - 1));
    }
#endif

    return m_pools.back()->allocate();
}

#ifdef CAPTAL_DEBUG
void render_technique::set_name(std::string_view name)
{
    m_name = name;

    tph::set_object_name(engine::instance().renderer(), m_descriptor_set_layout, m_name + " descriptor set layout");
    tph::set_object_name(engine::instance().renderer(), m_layout, m_name + " pipeline layout");
    tph::set_object_name(engine::instance().renderer(), m_pipeline, m_name + " pipeline");

    for(std::size_t i{}; i < std::size(m_pools); ++i)
    {
        m_pools[i]->set_name(m_name + " descriptor pool #" + std::to_string(std::size(m_pools) - 1));
    }
}
#endif

}
