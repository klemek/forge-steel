#ifndef CONFIG_H
#define CONFIG_H

#ifndef PACKAGE
#define PACKAGE "forge"
#endif /* PACKAGE */

#ifndef VERSION
#define VERSION "(dev)"
#endif /* VERSION */

#ifndef MAX_VIDEO
#define MAX_VIDEO 16
#endif

#ifndef MAX_FRAG
#define MAX_FRAG 64
#endif

#ifndef UNSET_MIDI_CODE
#define UNSET_MIDI_CODE 300
#endif

#ifndef MIDI_MAX
#define MIDI_MAX 127
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE 1024
#endif

#ifndef ARRAY_NOT_FOUND
#define ARRAY_NOT_FOUND ARRAY_SIZE + 1
#endif

#endif /* CONFIG_H */