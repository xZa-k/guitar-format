#include <stddef.h>
#include <stdint.h>


typedef enum {
  A = 0,
  As,  // A sharp
  B,
  C,
  Cs,  // C sharp
  D,
  Ds,  // D sharp
  E,
  F,
  Fs,  // F sharp
  G,
  Gs  // G sharp
} MusicalNote;

enum EventType { NoteOn = 0, NoteOff };

// typedef struct {
//   int noteCoord;
// } t_Note;

typedef volatile struct {
  char magic[4];
  char tuning[6];  // temp will change dynamically later
} GTabHeader;

typedef volatile struct {
  u_int16_t numerator;
  u_int16_t denominator;
  // char lol[2];
} TimeSignature;

typedef volatile struct {
  u_int16_t noteID;
  u_int32_t deltaTime;
} Event;

typedef volatile struct {
  u_int32_t count; // use uint32 instead of size_t incase multiplatform issues
  u_int32_t capacity;
  Event* items; // items at end, easier to unpack
} EventList;

typedef volatile struct {
  TimeSignature timeSignature;
  u_int32_t bpm; // bigger than needed for padding/alignment reasons
  EventList events;
} Bar;



typedef struct {
  GTabHeader header;
  Bar* bars;
} GuitarTab;