#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <log.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "timer.h"
#include "types.h"
#include "video.h"
#include "window.h"

static void ioctl_error(VideoCapture *video_capture, const char *operation,
                        const char *default_msg) {
  if (errno == EINVAL) {
    log_warn("(%s) %s -> EINVAL: %s", video_capture->name, operation,
             default_msg);
  } else if (errno == EAGAIN) {
    log_warn("(%s) %s -> EAGAIN: device state invalid", operation,
             video_capture->name);
  } else if (errno == EBADF) {
    log_warn("(%s) %s -> EBADF: file descriptor invalid", operation,
             video_capture->name);
  } else if (errno == EBUSY) {
    log_warn("(%s) %s -> EBUSY: device is busy", video_capture->name,
             operation);
  } else if (errno == EFAULT) {
    log_warn("(%s) %s -> EFAULT: invalid pointer", video_capture->name,
             operation);
  } else if (errno == ENODEV) {
    log_warn("(%s) %s -> ENODEV: device not found", video_capture->name,
             operation);
  } else if (errno == ENOMEM) {
    log_warn("(%s) %s -> ENOMEM: not enough memory", video_capture->name,
             operation);
  } else if (errno == ENOTTY) {
    log_warn("(%s) %s -> ENOTTY: ioctl not supported by file descriptor",
             video_capture->name, operation);
  } else if (errno == ENOSPC) {
    log_warn("(%s) %s -> ENOSPC: USB bandwidth error", video_capture->name,
             operation);
  } else if (errno == EPERM) {
    log_warn("(%s) %s -> EPERM: permission denied", video_capture->name,
             operation);
  } else if (errno == EIO) {
    log_warn("(%s) %s -> EIO: I/O error", video_capture->name, operation);
  } else if (errno == ENXIO) {
    log_warn("(%s) %s -> ENXIO: no device exists", video_capture->name,
             operation);
  } else if (errno == EPIPE) {
    log_warn("(%s) %s -> EPIPE: pipeline error", video_capture->name,
             operation);
  } else if (errno == ENOLINK) {
    log_warn("(%s) %s -> ENOLINK: pipeline configuration invalid for Media "
             "Controller interface",
             video_capture->name, operation);
  } else {
    log_error("(%s) %s unknown error %d", video_capture->name, operation,
              errno);
  }
  video_capture->error = true;
}

static VideoCapture open_device(char *name) {
  VideoCapture video_capture;

  video_capture.name = name;
  video_capture.error = false;
  video_capture.fd = -1;

  video_capture.fd = open(name, O_RDWR);
  if (video_capture.fd == -1) {
    log_warn("(%s) Cannot open device", name);
    video_capture.error = true;
  }

  return video_capture;
}

static bool check_caps(VideoCapture *video_capture) {
  struct v4l2_capability cap;

  memset(&cap, 0, sizeof(cap));

  if (ioctl(video_capture->fd, VIDIOC_QUERYCAP, &cap) == -1) {
    ioctl_error(video_capture, "VIDIOC_QUERYCAP", "Not a V4L2 device");
    return false;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    log_warn("(%s) Not a video capture device", video_capture->name);
    video_capture->error = true;
    return false;
  }

  if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    log_warn("(%s) No streaming i/o support", video_capture->name);
    video_capture->error = true;
    return false;
  }

  return true;
}

static bool get_available_sizes(VideoCapture *video_capture,
                                unsigned int preferred_height) {
  struct v4l2_frmsizeenum fmt_enum;
  unsigned int index;
  bool found = false;

  memset(&fmt_enum, 0, sizeof(fmt_enum));

  index = 0;
  fmt_enum.index = index;
  fmt_enum.pixel_format = V4L2_PIX_FMT_YUYV;

  found = false;
  video_capture->width = 0;
  video_capture->height = 0;

  while (ioctl(video_capture->fd, VIDIOC_ENUM_FRAMESIZES, &fmt_enum) == 0) {
    if (fmt_enum.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
      log_trace("(%s) %d: %dx%d", video_capture->name, index,
                fmt_enum.discrete.width, fmt_enum.discrete.height);

      if (fmt_enum.discrete.height == preferred_height) {
        video_capture->height = preferred_height;
        found = true;
        if (video_capture->width == 0 ||
            video_capture->width < fmt_enum.discrete.width) {
          video_capture->width = fmt_enum.discrete.width;
        }
      } else if (fmt_enum.discrete.height < preferred_height) {
        if (!found || fmt_enum.discrete.height > video_capture->height) {
          video_capture->height = fmt_enum.discrete.height;
          video_capture->width = fmt_enum.discrete.width;
          found = true;
        }
      } else if (video_capture->height == 0) {
        video_capture->height = fmt_enum.discrete.height;
        video_capture->width = fmt_enum.discrete.width;
      }
    }

    memset(&fmt_enum, 0, sizeof(fmt_enum));
    fmt_enum.index = ++index;
    fmt_enum.pixel_format = V4L2_PIX_FMT_YUYV;
  }

  if (video_capture->height == 0) {
    log_warn("(%s) No format found");
    video_capture->error = true;
    return false;
  }

  return true;
}

static bool set_format(VideoCapture *video_capture) {
  struct v4l2_format fmt;

  video_capture->output = false;

  memset(&fmt, 0, sizeof(fmt));

  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = video_capture->width;
  fmt.fmt.pix.height = video_capture->height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_ANY;

  if (ioctl(video_capture->fd, VIDIOC_S_FMT, &fmt) == -1) {
    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    video_capture->output = true;

    if (ioctl(video_capture->fd, VIDIOC_S_FMT, &fmt) == -1) {
      ioctl_error(video_capture, "VIDIOC_S_FMT",
                  "Requested buffer type not supported");
      return false;
    }
  }

  video_capture->width = fmt.fmt.pix.width;
  video_capture->height = fmt.fmt.pix.height;
  video_capture->pixelformat = fmt.fmt.pix.pixelformat;
  video_capture->bytesperline = fmt.fmt.pix.bytesperline;

  log_info("(%s) Format fourcc:  %c%c%c%c", video_capture->name,
           fmt.fmt.pix.pixelformat, fmt.fmt.pix.pixelformat >> 8,
           fmt.fmt.pix.pixelformat >> 16, fmt.fmt.pix.pixelformat >> 24);
  log_info("(%s) Resolution:  %dx%d", video_capture->name, fmt.fmt.pix.width,
           fmt.fmt.pix.height);

  return true;
}

static bool request_buffers(VideoCapture *video_capture) {
  struct v4l2_requestbuffers reqbuf;

  memset(&reqbuf, 0, sizeof(reqbuf));

  reqbuf.type = video_capture->output ? V4L2_BUF_TYPE_VIDEO_OUTPUT
                                      : V4L2_BUF_TYPE_VIDEO_CAPTURE;
  reqbuf.memory = V4L2_MEMORY_MMAP;
  reqbuf.count = 1;

  if (ioctl(video_capture->fd, VIDIOC_REQBUFS, &reqbuf) == -1) {
    ioctl_error(video_capture, "VIDIOC_REQBUFS",
                "Buffer type or I/O method not supported");
    return false;
  }

  log_info("(%s) V4L2 Buffer Count:  %d", video_capture->name, reqbuf.count);

  return true;
}

static bool export_buffer(VideoCapture *video_capture) {
  struct v4l2_exportbuffer expbuf;

  video_capture->exp_fd = -1;

  memset(&expbuf, 0, sizeof(expbuf));

  expbuf.type = video_capture->output ? V4L2_BUF_TYPE_VIDEO_OUTPUT
                                      : V4L2_BUF_TYPE_VIDEO_CAPTURE;
  expbuf.index = 0;
  expbuf.flags = O_RDONLY;

  if (ioctl(video_capture->fd, VIDIOC_EXPBUF, &expbuf) == -1) {
    ioctl_error(
        video_capture, "VIDIOC_EXPBUF",
        "A queue is not in MMAP mode or DMABUF exporting is not supported");
    return false;
  }

  video_capture->exp_fd = expbuf.fd;

  return true;
}

static const enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

static bool open_stream(VideoCapture *video_capture) {
  if (ioctl(video_capture->fd, VIDIOC_STREAMON, &buf_type) == -1) {
    ioctl_error(
        video_capture, "VIDIOC_STREAMON",
        "Buffer type not supported or no buffer allocated or enqueued yet");
    return false;
  }

  return true;
}

static void create_image_buffer(VideoCapture *video_capture) {
  memset(&video_capture->buf, 0, sizeof(video_capture->buf));

  video_capture->buf.type = video_capture->output ? V4L2_BUF_TYPE_VIDEO_OUTPUT
                                                  : V4L2_BUF_TYPE_VIDEO_CAPTURE;
  video_capture->buf.memory = V4L2_MEMORY_MMAP;
  video_capture->buf.index = 0;

  ioctl(video_capture->fd, VIDIOC_QBUF, &video_capture->buf);
}

static void close_stream(VideoCapture video_capture) {
  ioctl(video_capture.fd, VIDIOC_STREAMOFF, &buf_type);
}

VideoCapture video_init(char *name, unsigned int preferred_height) {
  VideoCapture video_capture;

  video_capture = open_device(name);

  if (video_capture.error) {
    return video_capture;
  }

  if (!check_caps(&video_capture)) {
    return video_capture;
  }

  if (!get_available_sizes(&video_capture, preferred_height)) {
    return video_capture;
  }

  if (!set_format(&video_capture)) {
    return video_capture;
  }

  if (!request_buffers(&video_capture)) {
    return video_capture;
  }

  if (!export_buffer(&video_capture)) {
    return video_capture;
  }

  if (!open_stream(&video_capture)) {
    return video_capture;
  }

  create_image_buffer(&video_capture);

  return video_capture;
}

static bool read_video(VideoCapture *video_capture) {
  if (ioctl(video_capture->fd, VIDIOC_DQBUF, &video_capture->buf) == -1) {
    ioctl_error(video_capture, "VIDIOC_DQBUF",
                "buffer type not supported or no buffer allocated or the index "
                "is out of bounds");
    return false;
  }

  if (ioctl(video_capture->fd, VIDIOC_QBUF, &video_capture->buf) == -1) {
    ioctl_error(video_capture, "VIDIOC_QBUF",
                "buffer type not supported or no buffer allocated or the index "
                "is out of bounds");
    return false;
  }

  return true;
}

void video_background_read(VideoCapture *video_capture, bool *stop) {
  pid_t pid;
  Timer timer;
  pid = fork();
  if (pid < 0) {
    log_error("Could not create subprocess");
    return;
  }
  if (pid == 0) {
    return;
  }
  log_info("%s background acquisition started (pid: %d)", video_capture->name,
           pid);
  timer = timer_init(30);

  while (!*stop && read_video(video_capture)) {
    // repeat infinitely
    if (timer_inc(&timer)) {
      log_trace("(%s) %.2ffps", video_capture->name, timer_reset(&timer));
    }
  }
  log_info("%s background acquisition stopped (pid: %d)", video_capture->name,
           pid);
  window_terminate();
  exit(EXIT_SUCCESS);
}

void video_free(VideoCapture video_capture) {
  if (!video_capture.error) {
    close_stream(video_capture);
  }
  if (video_capture.exp_fd != -1) {
    close(video_capture.exp_fd);
  }
  if (video_capture.fd != -1) {
    close(video_capture.fd);
  }
}