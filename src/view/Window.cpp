#include "Window.h"
#include "gtkmm/enums.h"

Window::Window() {
    set_title("Fourier Cam");
    set_default_size(800, 500);
    m_paned.set_orientation(Gtk::Orientation::HORIZONTAL);

    m_paned.set_start_child(m_settingsBox);
    m_paned.set_end_child(m_videoBox);

    set_child(m_paned);
}