#include "gtf.h"
#include "basicz.h"

#include <stddef.h>
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

size_t size_of_event(EventType type) {
  size_t size = 0;
  switch (type) {
    case SetTempo:
      size = sizeof(SetTempoEvent);
      break;
    case NoteOn:
      size = sizeof(NoteOnEvent);
      break;
    case SetTimeSignature:
      size = sizeof(SetTimeSignatureEvent);
      break;
  }
  return size;
}

char* note_beat(NoteOnEvent* event) {
  u_int32_t bpm = 120;

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
  create_event(tempoEvent, SetTempo, 0, 120);
  create_event(note1, NoteOn, 4, 0, 100);
  create_event(note2, NoteOn, 4, 4, 20);

  da_append(&events, tempoEvent);

  
  // test larger files
  for (int i = 0; i < 500; i++) {
    NoteOnEvent* event_data_notey = malloc(sizeof(NoteOnEvent));
    event_data_notey->noteID = 16;
    event_data_notey->deltaTime = i;
    event_data_notey->duration = 20;


    // Event* notey = {NoteOn, (void *)&(event_data_notey)};
    Event* notey = malloc(sizeof(Event));
    notey->type = NoteOn;
    notey->data = event_data_notey;
    da_append(&events, *notey);
  }

  // Event event = {NoteOn, 4};
  // Event event2 = {NoteOn, 10};

  da_append(&events, note1);
  da_append(&events, note2);

  // Capacity is only needed for runtime, so just write count
  fwrite(&events.count, sizeof(u_int32_t), 1, file);
  
  for (int i = 0; i < events.count; i++) {
    EventType type = events.items[i].type;
    size_t size = size_of_event(type);
    fwrite(&type, sizeof(EventType), 1, file);
    fwrite(events.items[i].data, size, 1, file);
  }

  // Unique code to specify the end of the file
  // char end_bytes[4] = {0xFF, 0xF1, 0xFF, 0xF1};
  // bytes_written = fwrite(end_bytes, sizeof(end_bytes), 1, file);

  fclose(file);
  return 1;
}

void read_events(char *buffer, size_t offset, EventList *eventList) {
  u_int32_t count;

  if (eventList->count == 0) {
    memcpy(&count, &buffer[offset], sizeof(u_int32_t));
    offset += sizeof(u_int32_t);
  } else {
    count = eventList->count;
    printf("nahhh\n");
  }
  

  printf("event count: %d\n", count);

  for (int i = 0; i < count; i++) {
    // if (offset >= 2048 - sizeof(EventType)) return;
    EventType type;
    memcpy(&type, &buffer[offset], sizeof(EventType));
    printf("Type: %x\n", type);
    offset += sizeof(EventType);
    if (offset >= 2048 - size_of_event(type)) return;


    void* data = malloc(size_of_event(type));
    
    memcpy(data, &buffer[offset], size_of_event(type));

    Event event;
    event.type = type;
    event.data = data;
    // printf("tempo: %d\n", ((SetTempoEvent*)event.data)->tempo);
    // eventList->items[0] = event;
    // eventList->count++;
    if (type == SetTempo) {
      printf("tempo: %d\n", ((SetTempoEvent*)(event.data))->tempo);
    } else if (type == NoteOn) {
      printf("Note deltaTime: %d\n", ((NoteOnEvent*)(event.data))->deltaTime);
    }
    da_append(eventList, event);

    offset += size_of_event(type);
  }
  // printf("data: %d\n", ((NoteOnEvent*)eventList->items[1].data)->noteID);
}

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

  EventList eventList = {0};
  do {
    read_events(buffer, i, &eventList);
    fread(buffer, sizeof(char), sizeof(buffer), file);
  }while (!feof(file));

  GuitarTab *tab;

  tab = malloc(sizeof(GuitarTab));

  // tab->bars = bar;
  tab->header = header;

  format_tab(tab);
  return 0;
}

int main() {
  printf("Hello!\n");
  #ifdef __GNUC__
    // note that clang 3.7 declares itself as a gcc 4.2"
    printf ("gcc detected version %d.%d\n", __GNUC__, __GNUC_MINOR__);
#endif

  GTabHeader header = {MAGIC_BYTES, {E, A, D, G, B, E}};
  write_gtab(&header);
  read_gtab();
  return 0;
}