#ifndef VIDEO_BOX_H
#define VIDEO_BOX_H

#include "gtkmm/drawingarea.h"
#include "gtkmm/glarea.h"
#include "gtkmm/paned.h"

class VideoBox : public Gtk::Paned {
    public:
        VideoBox();
    
    private:
        Gtk::DrawingArea m_webcamOutput;
        Gtk::GLArea m_fourierOutput;

        void draw_function(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        void on_glarea_realize();
        void on_glarea_unrealize();
        bool on_glarea_render(const Glib::RefPtr<Gdk::GLContext>& context);
};


#endif // VIDEO_BOX_H