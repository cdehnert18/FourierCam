#include "Window.h"
#include "gtkmm/enums.h"

Window::Window() {
    set_title("Fourier Cam");
    m_paned.set_orientation(Gtk::Orientation::HORIZONTAL);

    auto settings = Gtk::Settings::get_default();
    if (settings) {
        settings->property_gtk_application_prefer_dark_theme() = true;
    }

    m_paned.set_start_child(m_settingsBox);
    m_paned.set_end_child(m_videoBox);

    m_settingsBox.set_size_request(300, -1);

    set_child(m_paned);
}

void Window::set_available_video_sources(const std::vector<Glib::ustring>& sources) {
    m_settingsBox.set_video_sources(sources);
}

void Window::set_initial_video_size(int width, int height) {
    set_default_size(width + 300, height);
}

Glib::ustring Window::getSelectedVideoSource() {
    return m_settingsBox.getSelectedVideoSource();
}

void Window::start_video_stream() {
    m_videoBox.start_video_stream(m_settingsBox.getSelectedVideoSource(), *this);
}