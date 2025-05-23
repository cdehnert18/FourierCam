#include "Controller.h"
#include "model/VideoHandler.h"
#include "view/Window.h"

int Controller::run(int argc, char* argv[]) {
    
    app = Gtk::Application::create("de.cdehnert18.fourier-cam");
    
    app->signal_activate().connect([this] () { on_app_activate(); });

    return app->run(argc, argv);
}

void Controller::on_app_activate() {
    m_window = std::make_unique<Window>();
    
    VideoSource videoSource;
    m_window->set_available_video_sources(videoSource.get_available_video_sources());
    
    app->add_window(*m_window);
    m_window->start_video_stream();
    m_window->set_visible(true);
}