#include "VideoHandler.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <dirent.h>
#include <cstring>
#include <iostream>

VideoHandler::VideoHandler() {
    enumerate_video_devices();
}

std::vector<Glib::ustring> VideoHandler::get_video_sources() const {
    return m_video_sources;
}

void VideoHandler::enumerate_video_devices() {
    const char* dev_dir = "/dev";
    DIR* dir = opendir(dev_dir);
    if (!dir) {
        std::cerr << "Failed to open /dev directory" << std::endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strncmp(entry->d_name, "video", 5) == 0) {
            std::string device_path = std::string(dev_dir) + "/" + entry->d_name;

            if (is_video_capture_device(device_path)) {
                m_video_sources.push_back(device_path);
            }
        }
    }
    closedir(dir);
}

bool VideoHandler::is_video_capture_device(const Glib::ustring& device_path) const {
    int fd = open(device_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        return false;
    }

    v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));

    bool result = false;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) != -1) {
        if (cap.device_caps & V4L2_CAP_VIDEO_CAPTURE) {
            result = true;
        }
    }

    close(fd);
    return result;
}
