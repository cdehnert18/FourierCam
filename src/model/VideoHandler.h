#ifndef VIDEO_HANDLER_H
#define VIDEO_HANDLER_H

#include "glibmm/ustring.h"
#include <vector>

class VideoHandler {
public:
    VideoHandler();
    ~VideoHandler() = default;

    std::vector<Glib::ustring> get_video_sources() const;
    std::tuple<int, int> get_video_resolution(const Glib::ustring& device_path) const;

private:
    std::vector<Glib::ustring> m_video_sources;

    void enumerate_video_devices();
    bool is_video_capture_device(const Glib::ustring& device_path) const;
};

#endif // VIDEO_HANDLER_H