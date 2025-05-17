#include "VideoBox.h"
#include "gtkmm/drawingarea.h"
#include "gtkmm/enums.h"
#include <epoxy/gl.h>
#include <iostream>

VideoBox::VideoBox() : Gtk::Paned(Gtk::Orientation::VERTICAL) {

    m_webcamOutput.set_draw_func([this](const Cairo::RefPtr<Cairo::Context>& cr,
                                        int width, int height) {
        draw_function(cr, width, height);
    });

    m_webcamOutput.set_visible(true);

    // Configure GLArea
    m_fourierOutput.set_required_version(3, 2); // or lower if needed
    m_fourierOutput.set_auto_render(true);
    m_fourierOutput.set_hexpand(true);
    m_fourierOutput.set_vexpand(true);

    // Connect signals
    m_fourierOutput.signal_realize().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_realize));
    m_fourierOutput.signal_unrealize().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_unrealize));
    m_fourierOutput.signal_render().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_render), false);

    set_start_child(m_webcamOutput);
    set_end_child(m_fourierOutput);
}

void VideoBox::draw_function(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    cr->save();
    cr->set_source_rgb(0.2, 0.4, 0.6);
    cr->arc(width / 2.0, height / 2.0, std::min(width, height) / 2.5, 0, 2 * M_PI);
    cr->fill();
    cr->restore();
}

void VideoBox::setBoxSizes(std::tuple<int, int> dimensions) {
    m_webcamOutput.set_content_width(std::get<0>(dimensions) / 2);
    m_webcamOutput.set_content_height(std::get<1>(dimensions) / 2);
    m_fourierOutput.set_size_request(std::get<0>(dimensions) / 2, std::get<1>(dimensions) / 2);
}


void VideoBox::on_glarea_realize() {
    m_fourierOutput.make_current();

    if (m_fourierOutput.has_error()) {
        std::cerr << "GLArea failed to initialize OpenGL context" << std::endl;
        return;
    }

    // You can initialize shaders or buffers here if needed
}

void VideoBox::on_glarea_unrealize() {
    // Cleanup GL resources if any
}

bool VideoBox::on_glarea_render(const Glib::RefPtr<Gdk::GLContext>&) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // If rendering failed, return false
    return true;
}