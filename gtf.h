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
  u_int32_t deltaTime;
  u_int16_t noteID;
  u_int32_t duration;
} NoteOnEvent;

typedef struct {
  u_int32_t deltaTime;
  u_int32_t tempo;
} SetTempoEvent;

typedef struct {
  u_int32_t deltaTime;
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
  u_int32_t count; // use uint32 instead of size_t incase multiplatform issues
  u_int32_t capacity;
  Event *items; // items at end, easier to unpack
} EventList;

// typedef volatile struct {
//   TimeSignature timeSignature;
//   u_int32_t bpm; // bigger than needed for padding/alignment reasons
//   EventList events;
// } Bar;

typedef struct {
  GTabHeader header;
  EventList *events;
} GuitarTab;


#define create_event(name, type, ...) \
    type##Event event_data_##name = {__VA_ARGS__}; \
    Event name = {type, (void*)&(event_data_##name)}

