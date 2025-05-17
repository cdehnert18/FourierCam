#ifndef WINDOW_H
#define WINDOW_H

#include "component/SettingsBox.h"
#include "gtkmm/paned.h"
#include "view/component/SettingsBox.h"
#include "view/component/VideoBox.h"
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/paned.h>
#include <gtkmm.h>

class Window : public Gtk::Window {
public:
    Window();

    void set_video_sources(const std::vector<Glib::ustring>& sources);
    void setBoxSizes(std::tuple<int, int> dimensions);

    Glib::ustring getSelectedVideoSource();

private:
    Gtk::Paned m_paned;
    SettingsBox m_settingsBox;
    VideoBox m_videoBox;
};

#endif // WINDOW_H
