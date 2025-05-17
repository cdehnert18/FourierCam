#include "Controller.h"
#include "view/Window.h"

int Controller::run(int argc, char* argv[]) {
    auto app = Gtk::Application::create("de.cdehnert18.fourier-cam");
    return app->make_window_and_run<Window>(argc, argv);
}