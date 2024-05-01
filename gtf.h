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

typedef enum : u_int16_t {
  HammerOn = 0x0001,
  PullOff = 0x0002,
  SlideOn = 0x0004,
  SlideOff = 0x0008,
  VibratoOn = 0x0010,
  PalmMute = 0x0020,
  Tapping = 0x0040,
  Harmonic = 0x0080,
  BendHalf = 0x0100,
  BendFull = 0x0200,
} ArticulationFlags;

typedef volatile struct {
  u_int16_t numerator;
  u_int16_t denominator;
  // char lol[2];
} TimeSignature;

// Events start with FF
// Second byte is event type
typedef enum : u_int16_t {
  NoteOn = 0xFF01,
  SetTempo,
  SetTimeSignature,
} EventType;

typedef struct {
  u_int64_t deltaTime;
  u_int16_t noteID;
  u_int64_t duration;
  ArticulationFlags articulation;
} NoteOnEvent;

typedef struct {
  u_int64_t deltaTime;
  u_int32_t tempo;
} SetTempoEvent;

typedef struct {
  u_int64_t deltaTime;
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
  u_int32_t count; // use uint32 instead of size_t incase of multiplatform issues
  u_int32_t capacity;
  Event *items; // items at end, easier to unpack
} EventList;

typedef struct {
  GTabHeader header;
  EventList *events;
} GuitarTab;


#define create_event(name, type, ...) \
    type##Event event_data_##name = {__VA_ARGS__}; \
    Event name = {type, (void*)&(event_data_##name)}
