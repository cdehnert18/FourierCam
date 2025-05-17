#ifndef SETTINGS_BOX_H
#define SETTINGS_BOX_H

#include "gtkmm/dropdown.h"
#include "gtkmm/switch.h"
#include <gtkmm.h>

class SettingsBox : public Gtk::Box {
    public:
        SettingsBox();

        void set_video_sources(const std::vector<Glib::ustring>& sources);
        
    private:
        Gtk::DropDown m_videoSourceDropBox;
        Glib::RefPtr<Gtk::StringList> m_videoSources;
        Gtk::Switch m_grayScaleSwitch;
        Gtk::Button m_screenshotButton;
};

#endif // SETTINGS_BOX_H