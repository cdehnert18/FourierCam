#ifndef VIDEO_HANDLER_H
#define VIDEO_HANDLER_H

#include "glibmm/ustring.h"
#include <vector>
#include <tuple>

struct buffer {
    void* start;
    size_t length;
};

class VideoSource {
    public:
        VideoSource();
        ~VideoSource();

        std::vector<Glib::ustring> get_available_video_sources() const;
        std::vector<unsigned char> get_rgb_frame(const Glib::ustring& device_path);

        int openDevice(const Glib::ustring& device_path);
        void closeDevice();
        void get_video_resolution(int& width, int& height);
    
    private:
        std::vector<Glib::ustring> m_available_video_sources;

        int fd = -1;
        struct buffer* buffers = NULL;
        unsigned int n_buffers = 0;

        int m_width;
        int m_height;

        bool is_video_capture_device(const Glib::ustring& device_path) const;
        int xioctl(int fd, int request, void* arg);
        void errno_exit(const char* s);
};

#endif // VIDEO_HANDLER_H