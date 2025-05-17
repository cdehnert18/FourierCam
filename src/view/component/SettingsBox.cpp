#include "SettingsBox.h"
#include "glibmm/ustring.h"
#include "gtkmm/centerbox.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"

SettingsBox::SettingsBox() : Gtk::Box(Gtk::Orientation::VERTICAL) {

    // Container Styling
    set_margin(5);
    set_spacing(5);
    
    // Init components
    m_videoSources = Gtk::StringList::create({"Camera 1", "Camera 2", "Screen"});
    m_videoSourceDropBox.set_model(m_videoSources);

    m_grayScaleSwitch.set_active(false);
    m_screenshotButton.set_label("Take Screenshot");

    // Component Styling
    m_grayScaleSwitch.set_hexpand(false);
    m_grayScaleSwitch.set_halign(Gtk::Align::START);


    // Add widgets
    append(m_videoSourceDropBox);

    Gtk::CenterBox grayscaleBox = Gtk::CenterBox();
    grayscaleBox.set_margin_start(5);
    grayscaleBox.set_margin_end(5);
    Gtk::Label grayscaleLabel = Gtk::Label("Grayscale");
    grayscaleBox.set_start_widget(grayscaleLabel);
    grayscaleBox.set_end_widget(m_grayScaleSwitch);

    append(grayscaleBox);

    // Spacer
    Gtk::Box spacer = Gtk::Box(Gtk::Orientation::VERTICAL);
    spacer.set_vexpand(true);
    append(spacer);

    append(m_screenshotButton);


    // ClickHandler
    m_screenshotButton.signal_clicked().connect([this]() {
        // TODO
    });
}

void SettingsBox::set_video_sources(const std::vector<Glib::ustring>& sources) {
    m_videoSources = Gtk::StringList::create(sources);
    m_videoSourceDropBox.set_model(m_videoSources);
}

Glib::ustring SettingsBox::getSelectedVideoSource()  {
    int selectedIndex = m_videoSourceDropBox.get_selected();
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(m_videoSources->get_n_items())) {
        return m_videoSources->get_string(selectedIndex); // ✔️ wird kopiert
    }
    return ""; // leere Zeichenkette als Fallback
}
