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

enum EventType {
  NoteOn = 0,
  NoteOff
};

typedef struct {
  int noteCoord;
} t_Note;

typedef volatile struct {
  char magic[4];
  char tuning[6]; // temp will change dynamically later
} GTabHeader;

typedef struct {
  int bpm;
  int timeSignature;
  int* events;
} Bar;

typedef struct {
  
} t_Event;


typedef struct {
  GTabHeader header;
  Bar* bars;
} Guitar;