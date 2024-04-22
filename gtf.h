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
  char numerator;
  char denominator;
} TimeSignature;

typedef struct {
  int noteID;
  int deltaTime;
} Event;

typedef struct {
  size_t count;
  size_t capacity;
  Event* items; // items at end, easier to unpack
} EventList;

typedef struct {
  int bpm;
  TimeSignature timeSignature;
  EventList events;
} Bar;



typedef struct {
  GTabHeader header;
  Bar* bars;
} Guitar;