#ifndef CAPTAL_RENDER_WINDOW_HPP_INCLUDED
#define CAPTAL_RENDER_WINDOW_HPP_INCLUDED

#include "config.hpp"

#include <apyre/window.hpp>
#include <apyre/event.hpp>

#include <tephra/surface.hpp>
#include <tephra/swapchain.hpp>
#include <tephra/render_target.hpp>
#include <tephra/commands.hpp>

#include <sigslots/signal.hpp>

#include "render_target.hpp"
#include "color.hpp"

namespace cpt
{

struct video_mode
{
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t image_count{2};
    tph::present_mode present_mode{tph::present_mode::fifo};
    tph::sample_count sample_count{tph::sample_count::msaa_x1};
    tph::texture_format depth_format{tph::texture_format::undefined};
    bool clipping{true};
};

using window_event_signal = sigslot::signal<const apr::window_event&>;
using mouse_event_signal = sigslot::signal<const apr::mouse_event&>;
using keyboard_event_signal = sigslot::signal<const apr::keyboard_event&>;
using text_event_signal = sigslot::signal<const apr::text_event&>;

class CAPTAL_API render_window : apr::window, tph::surface, public render_target
{
public:
    render_window() = default;
    render_window(const std::string& title, const cpt::video_mode& mode, apr::window_options options = apr::window_options::none);
    render_window(const apr::monitor& monitor, const std::string& title, const cpt::video_mode& mode, apr::window_options options = apr::window_options::none);

    ~render_window();
    render_window(const render_window&) = delete;
    render_window& operator=(const render_window&) = delete;
    render_window(render_window&&) = default;
    render_window& operator=(render_window&&) = default;

    using apr::window::resize;
    using apr::window::change_limits;
    using apr::window::move;
    using apr::window::move_to;
    using apr::window::hide;
    using apr::window::show;
    using apr::window::hide_cursor;
    using apr::window::show_cursor;
    using apr::window::grab_cursor;
    using apr::window::release_cursor;
    using apr::window::minimize;
    using apr::window::maximize;
    using apr::window::enable_resizing;
    using apr::window::disable_resizing;
    using apr::window::restore;
    using apr::window::raise;
    using apr::window::change_title;
    using apr::window::change_icon;
    using apr::window::change_opacity;
    using apr::window::switch_to_fullscreen;
    using apr::window::switch_to_windowed_fullscreen;
    using apr::window::switch_to_windowed;

    using apr::window::id;
    using apr::window::width;
    using apr::window::height;
    using apr::window::x;
    using apr::window::y;
    using apr::window::has_focus;
    using apr::window::is_minimized;
    using apr::window::is_maximized;
    using apr::window::current_monitor;

    void update();
    void close();

    std::pair<tph::command_buffer&, frame_presented_signal&> begin_render();
    void present();

    void set_clear_color(const color& color) noexcept
    {
        m_clear_color = tph::clear_color_value{color.red, color.green, color.blue, color.alpha};
    }

    void set_clear_color(const tph::clear_color_value& color) noexcept
    {
        m_clear_color = color;
    }

    void set_clear_depth_stencil(float depth, std::uint32_t stencil) noexcept
    {
        m_clear_depth_stencil = tph::clear_depth_stencil_value{depth, stencil};
    }

    apr::window& get_window() noexcept
    {
        return *this;
    }

    const apr::window& get_window() const noexcept
    {
        return *this;
    }

    tph::surface& get_surface() noexcept
    {
        return *this;
    }

    const tph::surface& get_surface() const noexcept
    {
        return *this;
    }

    tph::swapchain& get_swapchain() noexcept
    {
        return m_swapchain;
    }

    const tph::swapchain& get_swapchain() const noexcept
    {
        return m_swapchain;
    }

    const cpt::video_mode& video_mode() const noexcept
    {
        return m_video_mode;
    }

    window_event_signal& on_gained_focus() noexcept {return m_gained_focus;}
    window_event_signal& on_lost_focus() noexcept {return m_lost_focus;}
    window_event_signal& on_mouse_entered() noexcept {return m_mouse_entered;}
    window_event_signal& on_mouse_left() noexcept {return m_mouse_left;}
    window_event_signal& on_moved() noexcept {return m_moved;}
    window_event_signal& on_resized() noexcept {return m_resized;}
    window_event_signal& on_minimized() noexcept {return m_minimized;}
    window_event_signal& on_maximized() noexcept {return m_maximized;}
    window_event_signal& on_restored() noexcept {return m_restored;}
    window_event_signal& on_close() noexcept {return m_close;}

    mouse_event_signal& on_mouse_button_pressed() noexcept {return m_mouse_button_pressed;}
    mouse_event_signal& on_mouse_button_released() noexcept {return m_mouse_button_released;}
    mouse_event_signal& on_mouse_moved() noexcept {return m_mouse_moved;}
    mouse_event_signal& on_mouse_wheel_scroll() noexcept {return m_mouse_wheel_scroll;}

    keyboard_event_signal& on_key_pressed() noexcept {return m_key_pressed;}
    keyboard_event_signal& on_key_released() noexcept {return m_key_released;}

    text_event_signal& on_text_entered() noexcept {return m_text_entered;}

    std::uint32_t frame_index() const noexcept
    {
        return m_frame_index;
    }

    bool is_closed() const noexcept
    {
        return m_closed;
    }

private:
    struct frame_data
    {
        tph::framebuffer framebuffer{};
        tph::command_pool pool{};
        tph::command_buffer buffer{};
        tph::semaphore image_available{};
        tph::semaphore image_presentable{};
        tph::fence fence{};
        frame_presented_signal signal{};
        bool begin{};
    };

private:
    void setup_frame_data();
    void setup_signals();
    void update_clear_values(tph::framebuffer& framebuffer);
    void wait_all();
    void recreate(const tph::surface_capabilities& capabilities);

private:
    tph::texture_format m_surface_format{};
    tph::swapchain m_swapchain{};
    tph::texture m_multisampling_texture{};
    tph::texture m_depth_texture{};
    std::vector<frame_data> m_frames_data{};
    cpt::video_mode m_video_mode{};
    std::uint32_t m_frame_index{};
    tph::clear_color_value m_clear_color{};
    tph::clear_depth_stencil_value m_clear_depth_stencil{};
    bool m_closed{};

    window_event_signal m_gained_focus{};
    window_event_signal m_lost_focus{};
    window_event_signal m_mouse_entered{};
    window_event_signal m_mouse_left{};
    window_event_signal m_moved{};
    window_event_signal m_resized{};
    window_event_signal m_minimized{};
    window_event_signal m_maximized{};
    window_event_signal m_restored{};
    window_event_signal m_close{};
    mouse_event_signal m_mouse_button_pressed{};
    mouse_event_signal m_mouse_button_released{};
    mouse_event_signal m_mouse_moved{};
    mouse_event_signal m_mouse_wheel_scroll{};
    keyboard_event_signal m_key_pressed{};
    keyboard_event_signal m_key_released{};
    text_event_signal m_text_entered{};
};

using render_window_ptr = std::shared_ptr<render_window>;
using render_window_weak_ptr = std::weak_ptr<render_window>;

}

#endif
