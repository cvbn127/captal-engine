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

#include "window.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "application.hpp"
#include "monitor.hpp"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_syswm.h>

#ifdef _WIN32
    #include <windowsx.h>
    #include <dwmapi.h>
#endif

namespace apr
{

static std::uint32_t to_sdl_options(window_options options) noexcept
{
    std::uint32_t output{SDL_WINDOW_VULKAN};

    if(static_cast<bool>(options & window_options::fullscreen))
    {
        output |= SDL_WINDOW_FULLSCREEN;
    }

    if(static_cast<bool>(options & window_options::hidden))
    {
        output |= SDL_WINDOW_HIDDEN;
    }

    if(static_cast<bool>(options & window_options::minimized))
    {
        output |= SDL_WINDOW_MINIMIZED;
    }

    if(static_cast<bool>(options & window_options::maximized))
    {
        output |= SDL_WINDOW_MAXIMIZED;
    }

    if(static_cast<bool>(options & window_options::high_dpi))
    {
        output |= SDL_WINDOW_ALLOW_HIGHDPI;
    }

    if(static_cast<bool>(options & window_options::extended_client_area))
    {
        return output | SDL_WINDOW_RESIZABLE;
    }
    else if(static_cast<bool>(options & window_options::borderless))
    {
        return output | SDL_WINDOW_BORDERLESS;
    }
    else if(static_cast<bool>(options & window_options::resizable))
    {
        return output | SDL_WINDOW_RESIZABLE;
    }

    return output;
}

static window_options filter_options(application& application, window_options options)
{
    if(!static_cast<bool>(application.extensions() & application_extension::extended_client_area))
    {
        options &= ~window_options::extended_client_area;
    }

    return options;
}

static SDL_HitTestResult hit_test_callback(SDL_Window*, const SDL_Point* area, void* data)
{
    auto& callback{*reinterpret_cast<window::hit_test_function_type*>(data)};

    return static_cast<SDL_HitTestResult>(callback(area->x, area->y));
}

#ifdef _WIN32
using winproc = LRESULT (CALLBACK *)(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

static LRESULT CALLBACK extended_client_area_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept
{
    if(message == WM_NCCALCSIZE && wparam == TRUE)
    {
        return 0;
    }

    const auto fallback{reinterpret_cast<winproc>(GetPropW(hwnd, L"cpt_sdl_winproc"))};

    return fallback(hwnd, message, wparam, lparam);
}
#endif

window::window(application& application, const std::string& title, std::uint32_t width, std::uint32_t height, window_options options)
:m_event_queue{&application.event_queue()}
,m_monitors{application.enumerate_monitors()}
,m_options{filter_options(application, options)}
,m_surface_size{std::make_unique<std::atomic<std::uint64_t>>()}
{
    m_window = SDL_CreateWindow(std::data(title), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, static_cast<int>(width), static_cast<int>(height), to_sdl_options(options));

    if(!m_window)
        throw std::runtime_error{"Can not create window. " + std::string{SDL_GetError()}};

    m_event_queue->register_window(id());

    #ifdef _WIN32
    if(static_cast<bool>(m_options & window_options::extended_client_area))
    {
        const auto hwnd{reinterpret_cast<HWND>(std::get<win32_window_info>(platform_info()).window)};

        SetPropW(hwnd, L"cpt_sdl_winproc", reinterpret_cast<HANDLE>(GetWindowLongPtrW(hwnd, GWLP_WNDPROC)));
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&extended_client_area_proc));

        const MARGINS margin{1, 1, 1, 1};
        DwmExtendFrameIntoClientArea(hwnd, &margin);

        RECT rect{};
        GetWindowRect(hwnd, &rect);
        SetWindowPos(hwnd, nullptr, rect.left, rect.top, static_cast<int>(width), static_cast<int>(height), SWP_FRAMECHANGED);
    }
    #endif
}

window::window(application& application, const monitor& monitor, const std::string& title, std::uint32_t width, std::uint32_t height, window_options options)
:window{application, title, width, height, options}
{
    const std::int32_t x{monitor.x() + ((static_cast<std::int32_t>(monitor.width())  - static_cast<std::int32_t>(width))  / 2)};
    const std::int32_t y{monitor.y() + ((static_cast<std::int32_t>(monitor.height()) - static_cast<std::int32_t>(height)) / 2)};

    SDL_SetWindowPosition(m_window, x, y);
}

window::~window()
{
    close();
}

window::window(window&& other) noexcept
:m_window{std::exchange(other.m_window, nullptr)}
,m_event_queue{std::exchange(other.m_event_queue, nullptr)}
,m_monitors{other.m_monitors}
,m_options{other.m_options}
,m_hit_test_func{std::move(other.m_hit_test_func)}
,m_surface_size{std::move(other.m_surface_size)}
{
    if(m_hit_test_func)
    {
        SDL_SetWindowHitTest(m_window, hit_test_callback, &m_hit_test_func);
    }
}

window& window::operator=(window&& other) noexcept
{
    close();

    m_window = other.m_window;
    m_event_queue = other.m_event_queue;
    m_monitors = other.m_monitors;
    m_options = other.m_options;
    m_hit_test_func = std::move(other.m_hit_test_func);
    m_surface_size = std::move(other.m_surface_size);

    if(m_hit_test_func)
    {
        SDL_SetWindowHitTest(m_window, hit_test_callback, &m_hit_test_func);
    }

    return *this;
}

void window::close() noexcept
{
    if(m_window)
    {
        m_event_queue->unregister_window(id());

        SDL_DestroyWindow(std::exchange(m_window, nullptr));
    }
}

void window::resize(std::uint32_t width, std::uint32_t height)
{
    #ifdef _WIN32
    if(static_cast<bool>(m_options & window_options::extended_client_area))
    {
        const auto hwnd{reinterpret_cast<HWND>(std::get<win32_window_info>(platform_info()).window)};

        RECT rect{};
        GetWindowRect(hwnd, &rect);
        SetWindowPos(hwnd, nullptr, rect.left, rect.top, static_cast<int>(width), static_cast<int>(height), SWP_FRAMECHANGED);
    }
    else
    {
        SDL_SetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
    }
    #else
    SDL_SetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
    #endif
}

void window::change_limits(std::uint32_t min_width, std::uint32_t min_height, std::uint32_t max_width, std::uint32_t max_height)
{
    SDL_SetWindowMinimumSize(m_window, static_cast<int>(min_width), static_cast<int>(min_height));
    SDL_SetWindowMaximumSize(m_window, static_cast<int>(max_width), static_cast<int>(max_height));
}

void window::move(std::int32_t relative_x, std::int32_t relative_y)
{
    move_to(x() + relative_x, y() + relative_y);
}

void window::move_to(std::int32_t x, std::int32_t y)
{
    #ifdef _WIN32
    if(static_cast<bool>(m_options & window_options::extended_client_area))
    {
        const auto hwnd{reinterpret_cast<HWND>(std::get<win32_window_info>(platform_info()).window)};

        RECT rect{};
        GetWindowRect(hwnd, &rect);
        const auto width {rect.right - rect.left};
        const auto height{rect.bottom - rect.top};

        SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_FRAMECHANGED);
    }
    else
    {
        SDL_SetWindowPosition(m_window, x, y);
    }
    #else
    SDL_SetWindowPosition(m_window, x, y);
    #endif
}

void window::move_to(const monitor& monitor, std::int32_t x, std::int32_t y)
{
    move_to(monitor.x() + x, monitor.y() + y);
}

void window::hide()
{
    SDL_HideWindow(m_window);
}

void window::show()
{
    SDL_ShowWindow(m_window);
}

void window::hide_cursor()
{
    SDL_ShowCursor(SDL_DISABLE);
}

void window::show_cursor()
{
    SDL_ShowCursor(SDL_ENABLE);
}

void window::grab_cursor()
{
    SDL_SetWindowGrab(m_window, SDL_TRUE);
}

void window::release_cursor()
{
    SDL_SetWindowGrab(m_window, SDL_FALSE);
}

void window::minimize()
{
    SDL_MinimizeWindow(m_window);
}

void window::maximize()
{
    SDL_MaximizeWindow(m_window);
}

void window::enable_resizing()
{
    SDL_SetWindowResizable(m_window, SDL_TRUE);
}

void window::disable_resizing()
{
    SDL_SetWindowResizable(m_window, SDL_FALSE);
}

void window::restore()
{
    SDL_RestoreWindow(m_window);
}

void window::raise()
{
    SDL_RaiseWindow(m_window);
}

void window::change_title(const std::string& title)
{
    SDL_SetWindowTitle(m_window, reinterpret_cast<const char*>(std::data(title)));
}

void window::change_icon(const std::uint8_t* rgba, std::uint32_t width, std::uint32_t height)
{
    SDL_Surface* surface{SDL_CreateRGBSurfaceFrom(const_cast<std::uint8_t*>(rgba), static_cast<int>(width), static_cast<int>(height), 32, width * 4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF)};
    if(!surface)
        throw std::runtime_error{"Can not set window icon." + std::string{SDL_GetError()}};

    SDL_SetWindowIcon(m_window, surface);

    SDL_FreeSurface(surface);
}

void window::change_opacity(float opacity)
{
    SDL_SetWindowOpacity(m_window, opacity);
}

void window::change_hit_test_function(hit_test_function_type func)
{
    if(func)
    {
        m_hit_test_func = std::move(func);
        SDL_SetWindowHitTest(m_window, hit_test_callback, &m_hit_test_func);
    }
    else
    {
        m_hit_test_func = nullptr;
        SDL_SetWindowHitTest(m_window, nullptr, nullptr);
    }
}

void window::switch_to_fullscreen()
{
    #ifdef _WIN32
    if(static_cast<bool>(m_options & window_options::extended_client_area) && !m_need_fullscreen_restore)
    {
        if(!static_cast<bool>(SDL_GetWindowFlags(m_window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)))
        {
            const auto    hwnd  {reinterpret_cast<HWND>(std::get<win32_window_info>(platform_info()).window)};
            const MARGINS margin{-1, -1, -1, -1};

            RECT rect{};
            GetWindowRect(hwnd, &rect);
            m_windowed_width  = rect.right - rect.left;
            m_windowed_height = rect.bottom - rect.top;

            DwmExtendFrameIntoClientArea(hwnd, &margin);

            GetWindowRect(hwnd, &rect);
            SetWindowPos(hwnd, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED);
        }
    }
    #endif

    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
}

void window::switch_to_fullscreen(const monitor& monitor)
{
    move_to(monitor, 0, 0);
    switch_to_fullscreen();
}

void window::switch_to_windowed_fullscreen()
{
    #ifdef _WIN32
    if(static_cast<bool>(m_options & window_options::extended_client_area))
    {
        if(!static_cast<bool>(SDL_GetWindowFlags(m_window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)))
        {
            const auto    hwnd  {reinterpret_cast<HWND>(std::get<win32_window_info>(platform_info()).window)};
            const MARGINS margin{-1, -1, -1, -1};

            RECT rect{};
            GetWindowRect(hwnd, &rect);
            m_windowed_width  = rect.right - rect.left;
            m_windowed_height = rect.bottom - rect.top;

            DwmExtendFrameIntoClientArea(hwnd, &margin);

            GetWindowRect(hwnd, &rect);
            SetWindowPos(hwnd, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED);
        }
    }
    #endif

    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void window::switch_to_windowed_fullscreen(const monitor& monitor)
{
    move_to(monitor, 0, 0);
    switch_to_windowed_fullscreen();
}

void window::switch_to_windowed()
{
    #ifdef _WIN32
    const bool need_dwm_process{static_cast<bool>(SDL_GetWindowFlags(m_window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP))};
    #endif

    SDL_SetWindowFullscreen(m_window, 0);

    #ifdef _WIN32
    if(static_cast<bool>(m_options & window_options::extended_client_area) && !m_need_fullscreen_restore)
    {
        if(need_dwm_process)
        {
            const auto    hwnd  {reinterpret_cast<HWND>(std::get<win32_window_info>(platform_info()).window)};
            const MARGINS margin{1, 1, 1, 1};

            DwmExtendFrameIntoClientArea(hwnd, &margin);

            RECT rect{};
            GetWindowRect(hwnd, &rect);
            SetWindowPos(hwnd, nullptr, rect.left, rect.top, m_windowed_width, m_windowed_height, SWP_FRAMECHANGED);
        }
    }
    #endif
}

void window::switch_to_windowed(const monitor& monitor)
{
    switch_to_windowed();
    move_to(monitor, 0, 0);
}

VkSurfaceKHR window::make_surface(VkInstance instance)
{
    VkSurfaceKHR surface{};

    if(!SDL_Vulkan_CreateSurface(m_window, instance, &surface))
        throw std::runtime_error{"Can not create window surface. " + std::string{SDL_GetError()}};

    return surface;
}

window::id_type window::id() const noexcept
{
    return m_window ? SDL_GetWindowID(m_window) : 0;
}

std::uint32_t window::width() const noexcept
{
    int output{};
    SDL_GetWindowSize(m_window, &output, nullptr);
    return static_cast<std::uint32_t>(output);
}

std::uint32_t window::height() const noexcept
{
    int output{};
    SDL_GetWindowSize(m_window, nullptr, &output);
    return static_cast<std::uint32_t>(output);
}

std::int32_t window::x() const noexcept
{
    int output{};
    SDL_GetWindowPosition(m_window, &output, nullptr);
    return output;
}

std::int32_t window::x(const monitor& monitor) const noexcept
{
    int output{};
    SDL_GetWindowPosition(m_window, &output, nullptr);
    return output - monitor.x();
}

std::int32_t window::y() const noexcept
{
    int output{};
    SDL_GetWindowPosition(m_window, nullptr, &output);
    return output;
}

std::int32_t window::y(const monitor& monitor) const noexcept
{
    int output{};
    SDL_GetWindowPosition(m_window, nullptr, &output);
    return output - monitor.y();
}

bool window::has_focus() const noexcept
{
    return static_cast<bool>(SDL_GetWindowFlags(m_window) & (SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS));
}

bool window::is_minimized() const noexcept
{
    return static_cast<bool>(SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED);
}

bool window::is_maximized() const noexcept
{
    return static_cast<bool>(SDL_GetWindowFlags(m_window) & SDL_WINDOW_MAXIMIZED);
}

const monitor& window::current_monitor() const noexcept
{
    return m_monitors[SDL_GetWindowDisplayIndex(m_window)];
}

platform_window_info window::platform_info() const noexcept
{
    SDL_SysWMinfo info{};
    info.version.major = SDL_MAJOR_VERSION;
    info.version.minor = SDL_MINOR_VERSION;
    info.version.patch = SDL_PATCHLEVEL;

    SDL_GetWindowWMInfo(m_window, &info);

#if defined(SDL_VIDEO_DRIVER_WINDOWS)

    return platform_window_info{win32_window_info{info.info.win.hinstance, info.info.win.hdc, info.info.win.window}};

#elif defined(SDL_VIDEO_DRIVER_X11) || defined(SDL_VIDEO_DRIVER_WAYLAND)

    switch(info.subsystem)
    {
    #if defined(SDL_VIDEO_DRIVER_X11)
        case SDL_SYSWM_X11:
            return platform_window_info{x11_window_info{info.info.x11.display, info.info.x11.window}};
    #elif defined(SDL_VIDEO_DRIVER_WAYLAND)
        case SDL_SYSWM_WAYLAND:
            return platform_window_info{wayland_window_info{info.info.wl.display, info.info.wl.surface, info.info.wl.shell_surface}};
    #endif
        default:
            throw std::runtime_error{"Invalid window subsystem."};
    }

#elif defined(SDL_VIDEO_DRIVER_COCOA)

    return platform_window_info{cocoa_window_info{info.info.cocoa.window}};

#elif defined(SDL_VIDEO_DRIVER_UIKIT)

    return platform_window_info{uikit_window_info{info.info.uikit.window}};

#elif defined(SDL_VIDEO_DRIVER_ANDROID)

    return platform_window_info{android_window_info{info.info.android.window}};

#endif

}

}
