#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <log.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "types.h"
#include "video.h"
#include "window.h"

static void ioctl_error(VideoDevice *device, const char *operation,
                        const char *default_msg) {
  if (errno == EINVAL) {
    log_warn("(%s) %s -> EINVAL: %s", device->name, operation, default_msg);
  } else if (errno == EAGAIN) {
    log_warn("(%s) %s -> EAGAIN: device state invalid", operation,
             device->name);
  } else if (errno == EBADF) {
    log_warn("(%s) %s -> EBADF: file descriptor invalid", operation,
             device->name);
  } else if (errno == EBUSY) {
    log_warn("(%s) %s -> EBUSY: device is busy", device->name, operation);
  } else if (errno == EFAULT) {
    log_warn("(%s) %s -> EFAULT: invalid pointer", device->name, operation);
  } else if (errno == ENODEV) {
    log_warn("(%s) %s -> ENODEV: device not found", device->name, operation);
  } else if (errno == ENOMEM) {
    log_warn("(%s) %s -> ENOMEM: not enough memory", device->name, operation);
  } else if (errno == ENOTTY) {
    log_warn("(%s) %s -> ENOTTY: ioctl not supported by file descriptor",
             device->name, operation);
  } else if (errno == ENOSPC) {
    log_warn("(%s) %s -> ENOSPC: USB bandwidth error", device->name, operation);
  } else if (errno == EPERM) {
    log_warn("(%s) %s -> EPERM: permission denied", device->name, operation);
  } else if (errno == EIO) {
    log_warn("(%s) %s -> EIO: I/O error", device->name, operation);
  } else if (errno == ENXIO) {
    log_warn("(%s) %s -> ENXIO: no device exists", device->name, operation);
  } else if (errno == EPIPE) {
    log_warn("(%s) %s -> EPIPE: pipeline error", device->name, operation);
  } else if (errno == ENOLINK) {
    log_warn("(%s) %s -> ENOLINK: pipeline configuration invalid for Media "
             "Controller interface",
             device->name, operation);
  } else {
    log_error("(%s) %s unknown error %d", device->name, operation, errno);
  }
  device->error = true;
}

static VideoDevice open_device(char *name) {
  VideoDevice device;

  device.name = name;
  device.error = false;
  device.fd = -1;

  device.fd = open(name, O_RDWR);
  if (device.fd == -1) {
    log_warn("(%s) Cannot open device", name);
    device.error = true;
  }

  return device;
}

static bool check_device_caps(VideoDevice *device) {
  struct v4l2_capability cap;

  memset(&cap, 0, sizeof(cap));

  if (ioctl(device->fd, VIDIOC_QUERYCAP, &cap) == -1) {
    ioctl_error(device, "VIDIOC_QUERYCAP", "Not a V4L2 device");
    return false;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    log_warn("(%s) Not a video capture device", device->name);
    device->error = true;
    return false;
  }

  if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    log_warn("(%s) No streaming i/o support", device->name);
    device->error = true;
    return false;
  }

  return true;
}

static bool set_device_format(VideoDevice *device, unsigned int preferred_width,
                              unsigned int preferred_height) {
  struct v4l2_format fmt;

  device->output = false;

  memset(&fmt, 0, sizeof(fmt));

  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = preferred_width;
  fmt.fmt.pix.height = preferred_height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

  if (ioctl(device->fd, VIDIOC_S_FMT, &fmt) == -1) {
    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    device->output = true;

    if (ioctl(device->fd, VIDIOC_S_FMT, &fmt) == -1) {
      ioctl_error(device, "VIDIOC_S_FMT",
                  "Requested buffer type not supported");
      return false;
    }
  }

  device->width = fmt.fmt.pix.width;
  device->height = fmt.fmt.pix.height;
  device->pixelformat = fmt.fmt.pix.pixelformat;
  device->bytesperline = fmt.fmt.pix.bytesperline;

  log_info("(%s) Format fourcc:  %c%c%c%c", device->name,
           fmt.fmt.pix.pixelformat, fmt.fmt.pix.pixelformat >> 8,
           fmt.fmt.pix.pixelformat >> 16, fmt.fmt.pix.pixelformat >> 24);
  log_info("(%s) Resolution:  %dx%d", device->name, fmt.fmt.pix.width,
           fmt.fmt.pix.height);

  return true;
}

static bool request_buffers(VideoDevice *device) {
  struct v4l2_requestbuffers reqbuf;

  memset(&reqbuf, 0, sizeof(reqbuf));

  reqbuf.type =
      device->output ? V4L2_BUF_TYPE_VIDEO_OUTPUT : V4L2_BUF_TYPE_VIDEO_CAPTURE;
  reqbuf.memory = V4L2_MEMORY_MMAP;
  reqbuf.count = 1;

  if (ioctl(device->fd, VIDIOC_REQBUFS, &reqbuf) == -1) {
    ioctl_error(device, "VIDIOC_REQBUFS",
                "Buffer type or I/O method not supported");
    return false;
  }

  log_info("(%s) V4L2 Buffer Count:  %d", device->name, reqbuf.count);

  return true;
}

static bool export_buffer(VideoDevice *device) {
  struct v4l2_exportbuffer expbuf;

  device->exp_fd = -1;

  memset(&expbuf, 0, sizeof(expbuf));

  expbuf.type =
      device->output ? V4L2_BUF_TYPE_VIDEO_OUTPUT : V4L2_BUF_TYPE_VIDEO_CAPTURE;
  expbuf.index = 0;
  expbuf.flags = O_RDONLY;

  if (ioctl(device->fd, VIDIOC_EXPBUF, &expbuf) == -1) {
    ioctl_error(
        device, "VIDIOC_EXPBUF",
        "A queue is not in MMAP mode or DMABUF exporting is not supported");
    return false;
  }

  device->exp_fd = expbuf.fd;

  return true;
}

static const enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

static bool open_stream(VideoDevice *device) {
  if (ioctl(device->fd, VIDIOC_STREAMON, &buf_type) == -1) {
    ioctl_error(
        device, "VIDIOC_STREAMON",
        "Buffer type not supported or no buffer allocated or enqueued yet");
    return false;
  }

  return true;
}

static void create_image_buffer(VideoDevice *device) {
  memset(&device->buf, 0, sizeof(device->buf));

  device->buf.type =
      device->output ? V4L2_BUF_TYPE_VIDEO_OUTPUT : V4L2_BUF_TYPE_VIDEO_CAPTURE;
  device->buf.memory = V4L2_MEMORY_MMAP;
  device->buf.index = 0;

  ioctl(device->fd, VIDIOC_QBUF, &device->buf);
}

static void close_stream(VideoDevice device) {
  ioctl(device.fd, VIDIOC_STREAMOFF, &buf_type);
}

VideoDevice video_init(char *name, unsigned int preferred_width,
                       unsigned int preferred_height) {
  VideoDevice device;

  device = open_device(name);

  if (device.error) {
    return device;
  }

  if (!check_device_caps(&device)) {
    return device;
  }

  if (!set_device_format(&device, preferred_width, preferred_height)) {
    return device;
  }

  if (!request_buffers(&device)) {
    return device;
  }

  if (!export_buffer(&device)) {
    return device;
  }

  if (!open_stream(&device)) {
    return device;
  }

  create_image_buffer(&device);

  return device;
}

static bool read_video(VideoDevice *device) {
  if (ioctl(device->fd, VIDIOC_DQBUF, &device->buf) == -1) {
    ioctl_error(device, "VIDIOC_DQBUF",
                "buffer type not supported or no buffer allocated or the index "
                "is out of bounds");
    return false;
  }

  if (ioctl(device->fd, VIDIOC_QBUF, &device->buf) == -1) {
    ioctl_error(device, "VIDIOC_QBUF",
                "buffer type not supported or no buffer allocated or the index "
                "is out of bounds");
    return false;
  }

  return true;
}

void video_background_read(VideoDevice *device, bool *stop) {
  pid_t pid;
  pid = fork();
  if (pid < 0) {
    log_error("Could not create subprocess");
    return;
  }
  if (pid == 0) {
    return;
  }
  log_info("%s background acquisition started (pid: %d)", device->name, pid);
  while (!*stop && read_video(device)) {
    // repeat infinitely
  }
  log_info("%s background acquisition stopped (pid: %d)", device->name, pid);
  window_terminate();
  exit(EXIT_SUCCESS);
}

void video_free(VideoDevice device) {
  if (!device.error) {
    close_stream(device);
  }
  if (device.exp_fd != -1) {
    close(device.exp_fd);
  }
  if (device.fd != -1) {
    close(device.fd);
  }
}