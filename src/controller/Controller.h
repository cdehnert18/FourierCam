#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "glibmm/refptr.h"
#include "gtkmm/application.h"
#include "view/Window.h"
#include <gtkmm.h>

class Controller {
    public:
        int run(int argc, char* argv[]);

    private:
        Glib::RefPtr<Gtk::Application> app;
        std::unique_ptr<Window> m_window;
        
        void on_app_activate();
};

#endif // CONTROLLER_H
