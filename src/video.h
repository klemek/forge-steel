#include "types.h"

#ifndef VIDEO_H
#define VIDEO_H

void video_init(VideoCapture *video_capture, const char *name,
                unsigned int preferred_height);

bool video_background_read(VideoCapture *video_capture, SharedContext *context,
                           int input_index, bool trace_fps);

void video_free(const VideoCapture *video_capture);

#endif /* VIDEO_H */