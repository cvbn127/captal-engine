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

#ifndef CAPTAL_VIEW_HPP_INCLUDED
#define CAPTAL_VIEW_HPP_INCLUDED

#include "config.hpp"

#include <vector>
#include <numbers>

#include <captal_foundation/math.hpp>

#include "asynchronous_resource.hpp"
#include "push_constant_buffer.hpp"
#include "uniform_buffer.hpp"
#include "binding.hpp"
#include "render_technique.hpp"
#include "render_window.hpp"
#include "render_texture.hpp"

namespace cpt
{

enum class view_type
{
    orthographic = 0,
};

class CAPTAL_API view
{
public:
    struct uniform_data
    {
        mat4f view{identity};
        mat4f projection{identity};
    };

public:
    view() = default;
    explicit view(const render_target_ptr& target, const render_technique_info& info = render_technique_info{}, render_layout_ptr layout = nullptr, render_technique_options options = render_technique_options::none);
    explicit view(const render_target_ptr& target, render_technique_ptr technique);

    ~view() = default;
    view(const view&) = delete;
    view& operator=(const view&) = delete;
    view(view&&) noexcept = default;
    view& operator=(view&&) noexcept = default;

    void upload(memory_transfer_info info);
    void bind(frame_render_info info);

    void fit(std::uint32_t width, std::uint32_t height);
    void fit(const texture_ptr& window);
    void fit(const window_ptr& texture);

    void set_binding(std::uint32_t index, cpt::binding binding);

    template<typename T>
    void set_push_constant(tph::shader_stage stages, std::uint32_t offset, T&& value)
    {
        return m_push_constants.set(stages, offset, std::forward<T>(value));
    }

    void set_viewport(const tph::viewport& viewport) noexcept
    {
        m_viewport = viewport;
    }

    void set_scissor(const tph::scissor& scissor) noexcept
    {
        m_scissor = scissor;
    }

    void move_to(const vec3f& position) noexcept
    {
        m_position = position;
        update();
    }

    void move(const vec3f& relative) noexcept
    {
        m_position += relative;
        update();
    }

    void set_origin(const vec3f& origin) noexcept
    {
        m_origin = origin;
        update();
    }

    void move_origin(const vec3f& relative) noexcept
    {
        m_origin += relative;
        update();
    }

    void resize(float witdh, float height) noexcept
    {
        m_size = vec2f{witdh, height};
        update();
    }

    void set_z_near(float z_near) noexcept
    {
        m_z_near = z_near;
        update();
    }

    void set_z_far(float z_far) noexcept
    {
        m_z_far = z_far;
        update();
    }

    void set_rotation(float angle) noexcept
    {
        m_rotation = angle;
        update();
    }

    void rotate(float angle) noexcept
    {
        m_rotation = std::fmod(m_rotation + angle, std::numbers::pi_v<float> * 2.0f);
        update();
    }

    void set_scale(const vec3f& scale) noexcept
    {
        m_scale = scale;
        update();
    }

    void scale(const vec3f& scale) noexcept
    {
        m_scale *= scale;
        update();
    }

    void update() noexcept
    {
        m_need_upload = true;
    }

    render_target& target() const noexcept
    {
        return *m_target;
    }

    const render_technique_ptr& render_technique() const noexcept
    {
        return m_render_technique;
    }

    const cpt::binding& get_binding(std::uint32_t index) const noexcept
    {
        return m_bindings.get(index);
    }

    optional_ref<const cpt::binding> try_get_binding(std::uint32_t index) const noexcept
    {
        return m_bindings.try_get(index);
    }

    bool has_binding(std::uint32_t index) const noexcept
    {
        return m_bindings.has(index);
    }

    template<typename T>
    const T& get_push_constant(tph::shader_stage stages, std::uint32_t offset) const
    {
        return m_push_constants.get<T>(stages, offset);
    }

    template<typename T>
    optional_ref<const T> try_get_push_constant(tph::shader_stage stages, std::uint32_t offset) const
    {
        return m_push_constants.try_get<T>(stages, offset);
    }

    bool has_push_constant(tph::shader_stage stages, std::uint32_t offset) const
    {
        return m_push_constants.has(stages, offset);
    }

    const tph::viewport& viewport() const noexcept
    {
        return m_viewport;
    }

    const tph::scissor& scissor() const noexcept
    {
        return m_scissor;
    }

    const vec3f& position() const noexcept
    {
        return m_position;
    }

    const vec3f& origin() const noexcept
    {
        return m_origin;
    }

    float width() const noexcept
    {
        return m_size.x();
    }

    float height() const noexcept
    {
        return m_size.y();
    }

    float z_near() const noexcept
    {
        return m_z_near;
    }

    float z_far() const noexcept
    {
        return m_z_far;
    }

    float rotation() const noexcept
    {
        return m_rotation;
    }

    const vec3f& scale() const noexcept
    {
        return m_scale;
    }

    view_type type() const noexcept
    {
        return m_type;
    }

#ifdef CAPTAL_DEBUG
    void set_name(std::string_view name);
#else
    void set_name(std::string_view name [[maybe_unused]]) const noexcept
    {

    }
#endif

private:
    render_target* m_target{};
    render_technique_ptr m_render_technique{};
    binding_buffer m_bindings{};
    push_constants_buffer m_push_constants{};
    descriptor_set_ptr m_set{};
    std::vector<asynchronous_resource_ptr> m_to_keep{};

    tph::viewport m_viewport{};
    tph::scissor m_scissor{};
    vec3f m_position{};
    vec3f m_origin{};
    vec2f m_size{};
    float m_z_near{1.0f};
    float m_z_far{0.0f};
    vec3f m_scale{1.0f};
    float m_rotation{};
    view_type m_type{};

    bool m_need_upload{true};
    bool m_need_descriptor_update{true};

#ifdef CAPTAL_DEBUG
    std::string m_name{};
#endif
};

}

#endif
