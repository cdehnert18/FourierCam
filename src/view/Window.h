#ifndef WINDOW_H
#define WINDOW_H

#include <gtkmm.h>

class Window : public Gtk::Window {
public:
    Window();

private:
    void on_button_clicked();

    Gtk::Box vbox {Gtk::Orientation::VERTICAL};
    Gtk::Button button;
    Gtk::Label label;
};

#endif // WINDOW_H
