#include "gtf.h"
#include "basicz.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRETS 22

#define MAGIC_BYTES                                                            \
  { 0x47, 0x54, 0x41, 0x42 }

#define END_BYTES                                                              \
  { 0xFF, 0xF1, 0xFF, 0xF1 }

const char *note_enum_str(int note) {
  switch (note) {
  case A:
    return "A";
    break;
  case As:
    return "A#";
    break;
  case B:
    return "B";
    break;
  case C:
    return "C";
    break;
  case Cs:
    return "C#";
    break;
  case D:
    return "D";
    break;
  case Ds:
    return "D#";
    break;
  case E:
    return "E";
    break;
  case F:
    return "F";
    break;
  case Fs:
    return "F#";
    break;
  case G:
    return "G";
    break;
  case Gs:
    return "Gs";
    break;

  default:
    return "";
    break;
  }
}

void format_tab(GuitarTab *tab) {
  printf("tuning: %s\n", note_enum_str(tab->header.tuning[1]));
}

int write_gtab(GTabHeader *header) {
  FILE *file = fopen("mytab.gtab", "wb");

  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // Writes the header first
  printf("Header magic: %s\n", header->magic);
  size_t bytes_written = fwrite(header, sizeof(GTabHeader), 1, file);

  // test data
  TimeSignature timeSig = {4, 4};
  EventList events = {0};
  // Event tempoEvent = {SetTempoEvent, };

  // create event on stack, just for testing
  create_event(tempoEvent, SetTempo, 120);
  create_event(note1, NoteOn, 4, 0, 100);
  create_event(note2, NoteOn, 4, 4, 20);

  // Event event = {NoteOn, 4};
  // Event event2 = {NoteOn, 10};

  da_append(&events, tempoEvent);
  da_append(&events, note1);
  da_append(&events, note2);

  fwrite(&events, sizeof(u_int32_t) * 2, 1, file);
  for (int i = 0; i < events.count; i++) {
    EventType type = events.items[i].type;
    size_t size = 0;
    switch (type) {
    case SetTempo:
      size = sizeof(SetTempoEvent);
      break;
    case NoteOn:
      size = sizeof(NoteOnEvent);
      break;
    case SetTimeSignature:
      // <#code #>
      break;
    }
    fwrite(&type, sizeof(EventType), 1, file);
    fwrite(events.items[i].data, size, 1, file);
  }
  
  // Unique code to specify the end of the file
  char end_bytes[4] = {0xFF, 0xF1, 0xFF, 0xF1};
  // bytes_written = fwrite(end_bytes, sizeof(end_bytes), 1, file);

  fclose(file);
  return 1;
}
size_t reverse_endianness_size_t(size_t value) {
  size_t reversed_value = 0;
  size_t num_bytes = sizeof(value);

  for (size_t i = 0; i < num_bytes; ++i) {
    reversed_value |= ((value >> (8 * i)) & 0xFF) << (8 * (num_bytes - 1 - i));
  }

  return reversed_value;
}

// Bar *read_bar(char *buffer, size_t i) {
//   Bar* bar;
//   bar = malloc(sizeof(Bar));

//   memcpy(&bar->timeSignature, &buffer[i], sizeof(bar->timeSignature));
//   i += sizeof(bar->timeSignature);
//   memcpy(&bar->bpm, &buffer[i], sizeof(bar->bpm));
//   i += sizeof(bar->bpm);

//   printf("BPM: %d\n", bar->bpm);
//   printf("Time signature: %d/%d\n", bar->timeSignature.numerator,
//          bar->timeSignature.denominator);

//   printf("i: %zu\n", i);
//   // EventList eventList = {0};
//   // bar.events = eventList;

//   memcpy(&bar->events.count, &buffer[i], sizeof(u_int32_t));
//   printf("Event count: %u\n", bar->events.count);
//   i += sizeof(u_int32_t);

//   memcpy(&bar->events.capacity, &buffer[i], sizeof(u_int32_t));
//   printf("Capacity count: %u\n", bar->events.capacity);
//   i += sizeof(u_int32_t);

//   bar->events.items = malloc(bar->events.capacity);

//   for (size_t e = 0; e < bar->events.count; e++) {
//     Event event = {0};
//     memcpy(&event, &buffer[i + (e * sizeof(Event))], sizeof(Event));
//     bar->events.items[e] = event;
//   }
//   for (size_t j = 0; j < bar->events.count; j++) {
//     printf("note: %d, time %d\n", bar->events.items[j].noteID,
//            bar->events.items[j].deltaTime);
//   }

//   return bar;
// }

void read_events(char *buffer, size_t i, EventList *eventList) {}

int read_gtab() {
  char buffer[2048];

  FILE *file = fopen("mytab.gtab", "rb");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  size_t bytes_read = fread(buffer, sizeof(char), sizeof(buffer), file);
  if (bytes_read == 0) {
    perror("Error reading file");
    fclose(file);
    return 1;
  }

  char magic[4] = MAGIC_BYTES;
  if (memcmp(&buffer[0], magic, sizeof(magic))) {
    printf("Invalid GTab file\n");
    return 1;
  }

  int i = 0 + sizeof(magic); // skip magic
  int end = 0;
  char end_bytes[4] = END_BYTES;

  GTabHeader header = {};

  memcpy(header.tuning, &buffer[i], sizeof(header.tuning));
  i += sizeof(header.tuning);

  printf("the tuning is: ");
  for (size_t i = 0; i < sizeof(header.tuning); i++) {
    printf("%s ", note_enum_str(header.tuning[i]));
  }
  printf("\n");

  // Bar* bar = read_bar(&buffer, i);
  EventList *eventList = {0};
  read_events(buffer, i, eventList);

  GuitarTab *tab;

  tab = malloc(sizeof(GuitarTab));

  // tab->bars = bar;
  tab->header = header;

  format_tab(tab);
  return 0;
}

int main() {
  printf("Hello!\n");

  GTabHeader header = {MAGIC_BYTES, {E, A, D, G, B, E}};
  write_gtab(&header);
  read_gtab();
  return 0;
}