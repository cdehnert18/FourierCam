#ifndef VIDEO_HANDLER_H
#define VIDEO_HANDLER_H

#include "glibmm/ustring.h"
#include <vector>

struct buffer {
    void* start;
    size_t length;
};

class VideoHandler {
public:
    VideoHandler();
    ~VideoHandler() = default;

    std::vector<Glib::ustring> get_video_sources() const;
    std::tuple<int, int> get_video_resolution(const Glib::ustring& device_path) const;
    std::vector<unsigned char> get_rgb_frame(const Glib::ustring& device_path, int& width, int& height);

    int openDevice(const Glib::ustring& device_path);
    void closeDevice();
private:
    std::vector<Glib::ustring> m_video_sources;

    int fd = -1;
    struct buffer* buffers = NULL;
    unsigned int n_buffers = 0;

    int m_width;
    int m_height;

    void enumerate_video_devices();
    int xioctl(int fd, int request, void* arg);
    void errno_exit(const char* s);
    bool is_video_capture_device(const Glib::ustring& device_path) const;
};

#endif // VIDEO_HANDLER_H