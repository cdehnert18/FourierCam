#ifndef VIDEO_BOX_H
#define VIDEO_BOX_H

#include "gtkmm/glarea.h"
#include "gtkmm/paned.h"
#include "model/VideoHandler.h"
#include "view/shader/ShaderManager.h"
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <glib.h>
#include <vector>
#include <epoxy/gl.h>

class VideoBox : public Gtk::Paned {
    public:
        VideoBox();
        ~VideoBox();

        void set_initial_video_size(int width, int height);
        void start_video_stream(const Glib::ustring& device_path);
        void stop_video_stream();

    private:
        Gtk::GLArea m_webcamOutput;
        Gtk::GLArea m_fourierOutput;

        VideoHandler m_handler;
        ShaderManager m_shaderManager;
        std::chrono::steady_clock::time_point m_start_time;

        std::atomic<bool> m_running;
        std::thread m_video_thread;
        std::vector<unsigned char> m_frame_buffer;
        std::mutex m_frame_mutex;


        int m_initial_frame_width = 0;
        int m_initial_frame_height = 0;

        GLuint m_shader_program = 0;
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        GLuint m_texture_id = 0;

        void on_glarea_realize_webcam();
        void on_glarea_realize_fourier();
        void on_glarea_unrealize_webcam();
        void on_glarea_unrealize_fourier();
        bool on_glarea_render_fourier(const Glib::RefPtr<Gdk::GLContext>& context);
        bool on_glarea_render_webcam(const Glib::RefPtr<Gdk::GLContext>& context);
};


#endif // VIDEO_BOX_H