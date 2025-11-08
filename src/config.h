#ifndef CONFIG_H
#define CONFIG_H

/* PACKAGE */

#ifndef PACKAGE
#define PACKAGE "forge"
#endif

#ifndef VERSION
#define VERSION "steel-dev"
#endif

#ifndef DATADIR
#define DATADIR "."
#endif

/* STRINGS */

#ifndef STR_LEN
#define STR_LEN 2048
#endif

/* TYPES */

#ifndef MAX_VIDEO
#define MAX_VIDEO 16
#endif

#ifndef MAX_FRAG
#define MAX_FRAG 64
#endif

#ifndef MAX_SUB_FILE
#define MAX_SUB_FILE 63
#endif

/* MIDI */

#ifndef UNSET_MIDI_CODE
#define UNSET_MIDI_CODE 300
#endif

#ifndef MIDI_MAX
#define MIDI_MAX 127
#endif

/* ARRAY */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE 1024
#endif

#ifndef ARRAY_NOT_FOUND
#define ARRAY_NOT_FOUND ARRAY_SIZE + 1
#endif

/* TEMPO */

#ifndef MAX_BEAT_LENGTH
// 30.0 bpm
#define MAX_BEAT_LENGTH 2000
#endif

#ifndef MIN_BEAT_LENGTH
// 480.0 bpm
#define MIN_BEAT_LENGTH 125
#endif

#ifndef BEATS_UNTIL_CHAIN_RESET
#define BEATS_UNTIL_CHAIN_RESET 3
#endif

#ifndef TOTAL_TAP_VALUES
#define TOTAL_TAP_VALUES 8
#endif

#ifndef SKIPPED_TAP_THRESHOLD_LOW
#define SKIPPED_TAP_THRESHOLD_LOW 1.75
#endif

#ifndef SKIPPED_TAP_THRESHOLD_HIGH
#define SKIPPED_TAP_THRESHOLD_HIGH 2.75
#endif

#ifndef MAX_TAP_VALUES
#define MAX_TAP_VALUES 10
#endif

#endif /* CONFIG_H */