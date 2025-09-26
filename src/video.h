#include "types.h"

#ifndef VIDEO_H
#define VIDEO_H

VideoCapture video_init(char *name, unsigned int preferred_height);

void video_background_read(VideoCapture *video_capture, SharedBool *stop);

void video_free(VideoCapture video_capture);

#endif /* VIDEO_H */