#include "VideoBox.h"
#include "gtkmm/enums.h"
#include "model/VideoHandler.h"
#include <epoxy/gl.h>
#include <iostream>
#include <mutex>
#include "../shader/ShaderManager.h"
#include "../Window.h"

VideoBox::VideoBox() : Gtk::Paned(Gtk::Orientation::VERTICAL) {

    // Configure GLArea
    m_webcamOutput.set_required_version(4, 6);
    m_webcamOutput.set_auto_render(true);
    m_webcamOutput.set_hexpand(true);
    m_webcamOutput.set_vexpand(true);

    // Connect signals
    m_webcamOutput.signal_realize().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_realize_webcam));
    m_webcamOutput.signal_unrealize().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_unrealize_webcam));
    m_webcamOutput.signal_render().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_render_webcam), false);

    // Configure GLArea
    m_fourierOutput.set_required_version(4, 6);
    m_fourierOutput.set_auto_render(true);
    m_fourierOutput.set_hexpand(true);
    m_fourierOutput.set_vexpand(true);

    // Connect signals
    m_fourierOutput.signal_realize().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_realize_fourier));
    m_fourierOutput.signal_unrealize().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_unrealize_fourier));
    m_fourierOutput.signal_render().connect(sigc::mem_fun(*this, &VideoBox::on_glarea_render_fourier), false);

    m_start_time = std::chrono::steady_clock::now();

    set_start_child(m_webcamOutput);
    set_end_child(m_fourierOutput);
}

VideoBox::~VideoBox() {
    stop_video_stream();
}

void VideoBox::start_video_stream(const Glib::ustring& device_path, Window& parent) {
    
    if(m_videoSource.openDevice(device_path) < 0) {
            return;
    }
    m_videoSource.get_video_resolution(m_width, m_height);
    set_widget_size();
    parent.set_initial_video_size(m_width, m_height);
    m_running = true;
    m_video_thread = std::thread([this, device_path]() {
        while (m_running) {
            auto frame = m_videoSource.get_rgb_frame(device_path);
            if (!frame.empty()) {
                {
                    std::lock_guard<std::mutex> lock(m_frame_mutex);
                    m_frame_buffer = std::move(frame);
                }
                g_idle_add([](void* data) -> gboolean {
                    auto* self = static_cast<VideoBox*>(data);
                    self->m_webcamOutput.queue_draw();
                    self->m_fourierOutput.queue_draw();
                    return G_SOURCE_REMOVE;
                }, this);
            }
        }
        m_videoSource.closeDevice();
    });
}

void VideoBox::stop_video_stream() {
    m_running = false;
    if (m_video_thread.joinable()) {
        m_video_thread.join();
    }
}

void VideoBox::set_widget_size() {
    set_size_request(m_width, 2 * m_height);
    set_position(m_height);
    m_webcamOutput.set_size_request(m_width, m_height);
    m_fourierOutput.set_size_request(m_width, m_height);
}

void VideoBox::on_glarea_realize_webcam() {
    
    m_webcamOutput.make_current();

    if (m_webcamOutput.has_error()) {
        std::cerr << "GLArea failed to initialize OpenGL context" << std::endl;
        return;
    }    

    m_webcam_shader_program = m_shaderManager.create_shader_program("vertex.glsl", "fragment.glsl");

    float vertices[] = {
    // Positions   // TexCoords
    -1.f, -1.f,    0.f, 1.f,
     1.f, -1.f,    1.f, 1.f,
     1.f,  1.f,    1.f, 0.f,
    -1.f,  1.f,    0.f, 0.f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(m_webcam_shader_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    GLint texAttrib = glGetAttribLocation(m_webcam_shader_program, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void VideoBox::on_glarea_realize_fourier() {
    
    m_fourierOutput.make_current();

    if (m_fourierOutput.has_error()) {
        std::cerr << "GLArea failed to initialize OpenGL context" << std::endl;
        return;
    }    

    m_fourier_shader_program = m_shaderManager.create_shader_program("vertex.glsl", "dftFragment.glsl");

    float vertices[] = {
    // Positions   // TexCoords
    -1.f, -1.f,    0.f, 1.f,
     1.f, -1.f,    1.f, 1.f,
     1.f,  1.f,    1.f, 0.f,
    -1.f,  1.f,    0.f, 0.f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(m_fourier_shader_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    GLint texAttrib = glGetAttribLocation(m_fourier_shader_program, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void VideoBox::on_glarea_unrealize_webcam() {
    
    if (!m_webcamOutput.get_realized()) return;
    m_webcamOutput.make_current();
    
    if (m_texture_id != 0) {
        glDeleteTextures(1, &m_texture_id);
        m_texture_id = 0;
    }

    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if (m_webcam_shader_program != 0) {
        glDeleteProgram(m_webcam_shader_program);
        m_webcam_shader_program = 0;
    }
}

void VideoBox::on_glarea_unrealize_fourier() {
    if (!m_fourierOutput.get_realized()) return;
    m_fourierOutput.make_current();
    
    if (m_texture_id != 0) {
        glDeleteTextures(1, &m_texture_id);
        m_texture_id = 0;
    }

    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if (m_fourier_shader_program != 0) {
        glDeleteProgram(m_fourier_shader_program);
        m_fourier_shader_program = 0;
    }
}

bool VideoBox::on_glarea_render_webcam(const Glib::RefPtr<Gdk::GLContext>&) {
    std::lock_guard<std::mutex> lock(m_frame_mutex);

    auto now = std::chrono::steady_clock::now();
    float elapsed_seconds = std::chrono::duration<float>(now - m_start_time).count();


    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_frame_buffer.empty())
        return true;

    if (m_texture_id == 0) {
        glGenTextures(1, &m_texture_id);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_frame_buffer.data());

    glUseProgram(m_webcam_shader_program);
    GLint timeLoc = glGetUniformLocation(m_webcam_shader_program, "time");
    if (timeLoc != -1) {
        glUniform1f(timeLoc, elapsed_seconds);
    }


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    GLint texUniform = glGetUniformLocation(m_webcam_shader_program, "tex");
    glUniform1i(texUniform, 0);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);

    return true;
}

bool VideoBox::on_glarea_render_fourier(const Glib::RefPtr<Gdk::GLContext>&) {
    std::lock_guard<std::mutex> lock(m_frame_mutex);

    auto now = std::chrono::steady_clock::now();
    float elapsed_seconds = std::chrono::duration<float>(now - m_start_time).count();


    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_frame_buffer.empty())
        return true;

    if (m_texture_id == 0) {
        glGenTextures(1, &m_texture_id);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_frame_buffer.data());

    glUseProgram(m_fourier_shader_program);
    GLint timeLoc = glGetUniformLocation(m_fourier_shader_program, "time");
    if (timeLoc != -1) {
        glUniform1f(timeLoc, elapsed_seconds);
    }


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    GLint texUniform = glGetUniformLocation(m_fourier_shader_program, "tex");
    glUniform1i(texUniform, 0);

    GLint widthLoc = glGetUniformLocation(m_fourier_shader_program, "width");
    GLint heightLoc = glGetUniformLocation(m_fourier_shader_program, "height");
    if (widthLoc != -1) glUniform1i(widthLoc, m_width);
    if (heightLoc != -1) glUniform1i(heightLoc, m_height);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);

    return true;
}