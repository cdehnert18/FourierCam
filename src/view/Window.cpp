#include "Window.h"
#include "gtkmm/enums.h"

Window::Window() {
    set_title("Fourier Cam");
    set_default_size(800, 500);
    m_paned.set_orientation(Gtk::Orientation::HORIZONTAL);

    auto settings = Gtk::Settings::get_default();
    if (settings) {
        settings->property_gtk_application_prefer_dark_theme() = true;
    }

    m_paned.set_start_child(m_settingsBox);
    m_paned.set_end_child(m_videoBox);

    set_child(m_paned);
}

void Window::set_video_sources(const std::vector<Glib::ustring>& sources) {
    m_settingsBox.set_video_sources(sources);
}