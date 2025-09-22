#include "types.h"

#ifndef VIDEO_H
#define VIDEO_H

VideoDevice video_init(char *name, unsigned int preferred_width,
                       unsigned int preferred_height);

void video_background_read(VideoDevice *device, bool *stop);

void video_free(VideoDevice device);

#endif /* VIDEO_H */