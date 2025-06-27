#include "VideoHandler.h"

#include <algorithm>
#include <fcntl.h>
#include <ostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <dirent.h>
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdlib>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

VideoSource::VideoSource() {
    
    // get all available video sources
    DIR* dir = opendir("/dev");
    if (!dir) {
        std::cerr << "Failed to open /dev directory" << std::endl;
    } else {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strncmp(entry->d_name, "video", 5) == 0) {
                std::string device_path = std::string("/dev/") + entry->d_name;

                if (is_video_capture_device(device_path)) {
                    m_available_video_sources.push_back(device_path);
                }
            }
        }
        closedir(dir);
    }
}

VideoSource::~VideoSource() {
    closeDevice();
}

std::vector<Glib::ustring> VideoSource::get_available_video_sources() const {
    return m_available_video_sources;
}

bool VideoSource::is_video_capture_device(const Glib::ustring& device_path) const {
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

int VideoSource::xioctl(int fd, int request, void* arg) {
    int r;

    do r = ioctl (fd, request, arg);
    while (-1 == r && EINTR == errno);

    return r;
}

void VideoSource::errno_exit(const char* s) {
    fprintf (stderr, "%s error %d, %s\n",
                s, errno, strerror (errno));

    exit (EXIT_FAILURE);
}

int VideoSource::openDevice(const Glib::ustring& device_path) {
    
    struct stat st;

    if (-1 == stat (device_path.c_str(), &st)) {
            fprintf (stderr, "Cannot identify '%s': %d, %s\n", device_path.c_str(), errno, strerror (errno));
            exit (EXIT_FAILURE);
    }

    if (!S_ISCHR (st.st_mode)) {
            fprintf (stderr, "%s is no device\n", device_path.c_str());
            exit (EXIT_FAILURE);
    }

    fd = open (device_path.c_str(), O_RDWR /* required */, 0);

    if (-1 == fd) {
        fprintf (stderr, "Cannot open '%s': %d, %s\n", device_path.c_str(), errno, strerror (errno));
        exit (EXIT_FAILURE);
    }
    
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
	unsigned int min;

    if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
                fprintf (stderr, "%s is no V4L2 device\n", device_path.c_str());
                exit (EXIT_FAILURE);
        } else {
            errno_exit ("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf (stderr, "%s is no video capture device\n", device_path.c_str());
        exit (EXIT_FAILURE);
    }

    /* Select video input, video standard and tune here. */
	CLEAR (cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap)) {
            crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            crop.c = cropcap.defrect; /* reset to default */

            if (-1 == xioctl (fd, VIDIOC_S_CROP, &crop)) {
                    switch (errno) {
                    case EINVAL:
                            /* Cropping not supported. */
                            break;
                    default:
                            /* Errors ignored. */
                            break;
                    }
            }
    }

    CLEAR (fmt);

    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 320; 
    fmt.fmt.pix.height      = 240;
    //fmt.fmt.pix.width       = 1280; 
    //fmt.fmt.pix.height      = 720;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

    if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt)) {
        errno_exit ("VIDIOC_S_FMT");
    }

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;


    m_width = fmt.fmt.pix.width;
    m_height = fmt.fmt.pix.height;

    struct v4l2_requestbuffers req;

    CLEAR (req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf (stderr, "%s does not support " "memory mapping\n", device_path.c_str());
            exit (EXIT_FAILURE);
        } else {
            errno_exit ("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        fprintf (stderr, "Insufficient buffer memory on %s\n", device_path.c_str());
        exit (EXIT_FAILURE);
    }

    buffers = static_cast<buffer*>(calloc(req.count, sizeof(*buffers)));

    if (!buffers) {
        fprintf (stderr, "Out of memory\n");
        exit (EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;

        CLEAR (buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf)) 
            errno_exit ("VIDIOC_QUERYBUF");

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap (NULL /* start anywhere */,
                                    buf.length,
                                    PROT_READ | PROT_WRITE /* required */,
                                    MAP_SHARED /* recommended */,
                                    fd,
                                    buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            errno_exit ("mmap");
    }

    unsigned int i;
    enum v4l2_buf_type type;

    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR (buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
            errno_exit ("VIDIOC_QBUF");
    }
		
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
        errno_exit ("VIDIOC_STREAMON");

    return 1;
}

void VideoSource::closeDevice() {
    if (fd == -1)
        return;

    unsigned int i;
    for (i = 0; i < n_buffers; ++i) {
        if (-1 == munmap (buffers[i].start, buffers[i].length))
            errno_exit ("munmap");
    }

    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMOFF, &type);

    if (buffers != nullptr) {
        munmap(buffers->start, buffers->length);
        buffers->start = nullptr;
    }

    free(buffers);

    close(fd);
    fd = -1;
}

void VideoSource::get_video_resolution(int& width, int& height) {
    width = m_width;
    height = m_height;
}

std::vector<unsigned char> VideoSource::get_rgb_frame(const Glib::ustring& device_path) {
    if (fd == -1 || buffers->start == nullptr) {
        std::cerr << "Device not opened or buffer not initialized" << std::endl;
        return {};
    }

    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
            case EAGAIN:
                break;
            case EIO:
                break;
            default:
                errno_exit("VIDIOC_DQBUF");
        }
    }

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");

    unsigned char* yuyv = static_cast<unsigned char*>(buffers[buf.index].start);
    std::vector<unsigned char> rgb(m_width * m_height * 3);

    auto convert = [](int y, int u, int v) -> std::tuple<unsigned char, unsigned char, unsigned char> {
        int c = y - 16;
        int d = u - 128;
        int e = v - 128;

        int r = (298 * c + 409 * e + 128) >> 8;
        int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
        int b = (298 * c + 516 * d + 128) >> 8;

        r = std::clamp(r, 0, 255);
        g = std::clamp(g, 0, 255);
        b = std::clamp(b, 0, 255);

        return { static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b) };
    };

    for (int i = 0, j = 0; i < m_width * m_height * 2; i += 4, j += 6) {
        int y0 = yuyv[i];
        int u  = yuyv[i + 1];
        int y1 = yuyv[i + 2];
        int v  = yuyv[i + 3];

        auto [r0, g0, b0] = convert(y0, u, v);
        auto [r1, g1, b1] = convert(y1, u, v);

        rgb[j]     = r0;
        rgb[j + 1] = g0;
        rgb[j + 2] = b0;
        rgb[j + 3] = r1;
        rgb[j + 4] = g1;
        rgb[j + 5] = b1;
    }
    
    return rgb;
}


