#include <stddef.h>
#include <stdint.h>

typedef enum {
  A = 0,
  As, // A sharp
  B,
  C,
  Cs, // C sharp
  D,
  Ds, // D sharp
  E,
  F,
  Fs, // F sharp
  G,
  Gs // G sharp
} MusicalNote;

typedef enum {
  WholeNote,
  HalfNote,
  QuaterNote,
  EighthNote,
  SixteenthNote,
  ThirtySndNote
} NoteLength;

// [0] Hammer on
// [1] Pull off
// [2] Slide on
// [3] Slide off

// [4] Vibrato on
// [5] Palm mute
// [6] Tapping
// [7] Harmonic

// [8] Bend half
// [9] Bend full
// [10 - 15] Spare
typedef uint16_t ArticulationFlags;
#define HammerOn 0x0001
#define PullOff 0x0002
#define SlideOn 0x0004
#define SlideOff 0x0008
#define VibratoOn 0x0010
#define PalmMute 0x0020
#define Tapping 0x0040
#define Harmonic 0x0080
#define BendHalf 0x0100
#define BendFull 0x0200

typedef volatile struct {
  uint16_t numerator;
  uint16_t denominator;
  // char lol[2];
} TimeSignature;

// Events start with FF
// Second byte is event type
typedef uint16_t EventType;
#define NoteOn 0xFF01
#define SetTempo 0xFF02
#define SetTimeSignature 0xFF03

typedef struct {
  uint32_t deltaTime;
  uint32_t duration;
  uint16_t noteID;
  ArticulationFlags articulation;
} NoteOnEvent;

typedef struct {
  uint32_t deltaTime;
  uint32_t tempo;
} SetTempoEvent;

typedef struct {
  uint32_t deltaTime;
  TimeSignature timeSignature;
} SetTimeSignatureEvent;

typedef volatile struct {
  char magic[4];
  char tuning[6]; // temp will change dynamically later
} GTabHeader;

typedef volatile struct {
  EventType type;
  void *data; // depends on the type
} Event;

typedef volatile struct {
  uint32_t count; // use uint32 instead of size_t incase of multiplatform issues
  uint32_t capacity;
  Event *items; // items at end, easier to unpack
} EventList;

typedef struct {
  GTabHeader header;
  EventList *events;
} GuitarTab;

#define create_event(name, type, ...)                                          \
  type##Event event_data_##name = {__VA_ARGS__};                               \
  Event name = {type, (void *)&(event_data_##name)}

#define event_type_str(type) #type

#define eventlist_sort(arr, swap)                                              \
  do {                                                                         \
    typeof(*(arr)) tmp = {0};                                                  \
    tmp.count = (arr)->count;                                                  \
    tmp.capacity = (arr)->capacity;                                            \
    tmp.items = malloc(tmp.capacity * sizeof(*(arr)->items));                  \
    memcpy(tmp.items, (arr)->items, tmp.capacity * sizeof(*(arr)->items));     \
    if ((arr)->count <= 1)                                                     \
      break;                                                                   \
    for (size_t p = 0; p < tmp.count; p++) {                                   \
      for (size_t i = p + 1; i < tmp.count; i++) {                             \
        if ((arr)->items[p] > (arr)->items[i]) {                               \
          swap((arr)->items[p], (arr)->items[i]);                              \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)