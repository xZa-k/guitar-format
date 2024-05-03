#include "gtf.h"
#include "basicz.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MICRO_SECOND 1000000
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

int compare_event(Event* e1, Event* e2) {
  u_int32_t e1_delta_time;
  // first entry in data is always deltatime;
  memcpy(&e1_delta_time, e1->data, sizeof(u_int32_t));

  u_int32_t e2_delta_time;
  // first entry in data is always deltatime;
  memcpy(&e2_delta_time, e2->data, sizeof(u_int32_t));

  return e1_delta_time - e2_delta_time;

}



const char *type_to_string(EventType type) {
  const char *event_strings[] = {
      [NoteOn] = "NoteOn",
      [SetTempo] = "SetTempo",
      [SetTimeSignature] = "SetTimeSignature",
  };

  return event_strings[type];
}

const char *art_to_string(ArticulationFlags art) {
  const char *art_strings[] = {
      [HammerOn] = "HammerOn",   [PullOff] = "PullOff",
      [SlideOn] = "SlideOn",     [SlideOff] = "SlideOff",
      [VibratoOn] = "VibratoOn", [PalmMute] = "PalmMute",
      [Tapping] = "Tapping",     [Harmonic] = "Harmonic",
      [BendHalf] = "BendHalf",   [BendFull] = "BendFull",
  };

  return art_strings[art];
}

const char *length_to_string(NoteLength length) {
  const char *length_strings[] = {
      [WholeNote] = "WholeNote",         [HalfNote] = "HalfNote",
      [QuaterNote] = "QuaterNote",       [EighthNote] = "EighthNote",
      [SixteenthNote] = "SixteenthNote", [ThirtySndNote] = "ThirtySndNote",
  };

  return length_strings[length];
}

// ZString list_articulations(ArticulationFlags art) {

// }

u_int32_t get_quater_note(NoteOnEvent *event, TimeSignature timeSig,
                          u_int32_t tempo) {
  u_int64_t numer = timeSig.numerator * MICRO_SECOND;
  u_int64_t denom = timeSig.denominator * MICRO_SECOND;
  // u_int32_t fraction_of_measure = 1.f / denom;

  u_int64_t one_beat = (60 * MICRO_SECOND) / (tempo);
  printf("one beat: %llu\n", one_beat);

  u_int64_t note_length = (one_beat * numer) / denom;

  printf("q note: %llu\n", note_length);
  return note_length;
}

NoteLength note_type(NoteOnEvent *event, TimeSignature timeSig,
                     u_int32_t tempo) {
  float quater_note = get_quater_note(event, timeSig, tempo);

  // could use maths but this is easier to read/understand
  if (event->duration == quater_note * 4) {
    return WholeNote;
  } else if (event->duration == quater_note * 2) {
    return HalfNote;
  } else if (event->duration == quater_note) {
    return QuaterNote;
  } else if (event->duration == quater_note / 2) {
    return EighthNote;
  } else if (event->duration == quater_note / 4) {
    return SixteenthNote;
  } else if (event->duration == quater_note / 8) {
    return ThirtySndNote;
  } else {
    return ThirtySndNote;
  }
}

void print_events(GuitarTab *tab) {
  TimeSignature timeSig = {4, 4};

  printf("events:: \n");
  for (int i = 0; i < tab->events->count; i++) {
    Event e = tab->events->items[i];
    printf("{\ntype=%s,\n", type_to_string(e.type));

    switch (e.type) {
    case SetTempo:
      printf("[.deltaTime=%d,\n.tempo=%d] }\n",
             ((SetTempoEvent *)e.data)->deltaTime,
             ((SetTempoEvent *)e.data)->tempo);
      break;
    case NoteOn:
      printf("[.deltaTime=%d,\n.noteID=%d,\n.duration=%d(%s),\narticulation=%u]"
             "}\n",
             ((NoteOnEvent *)e.data)->deltaTime,
             ((NoteOnEvent *)e.data)->noteID, ((NoteOnEvent *)e.data)->duration,
             length_to_string(note_type(e.data, timeSig, 120)),
             ((NoteOnEvent *)e.data)->articulation);
      break;
    case SetTimeSignature:
      printf("[.deltaTime=%d,\n.numerator=%d,\n.denominator=%d]}\n",
             ((SetTimeSignatureEvent *)e.data)->deltaTime,
             ((SetTimeSignatureEvent *)e.data)->timeSignature.numerator,
             ((SetTimeSignatureEvent *)e.data)->timeSignature.denominator);

      break;
    }
    printf("\n");
  }
}


int write_gtab(GTabHeader *header) {
  FILE *file = fopen("mytab.gtab", "wb");

  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // Writes the header first
  size_t bytes_written = fwrite(header, sizeof(GTabHeader), 1, file);

  // test data
  TimeSignature timeSig = {4, 4};
  EventList events = {0};
  // Event tempoEvent = {SetTempoEvent, };

  // create event on stack, just for testing
  create_event(tempoEvent, SetTempo, 0, 120);
  create_event(note1, NoteOn, 99, 250000, 100);
  create_event(note2, NoteOn, 99, 125000, 20, HammerOn | PalmMute);

  da_append(&events, tempoEvent);

  // #ifdef TEST_MODE
  // test larger files
  for (int i = 0; i < 2000; i++) {
    NoteOnEvent *event_data_notey = malloc(sizeof(NoteOnEvent));
    event_data_notey->deltaTime = i;
    event_data_notey->duration = 500000;
    event_data_notey->noteID = 10;
    event_data_notey->articulation = 0;

    // Event* notey = {NoteOn, (void *)&(event_data_notey)};
    Event *notey = malloc(sizeof(Event));
    notey->type = NoteOn;
    notey->data = event_data_notey;
    // printf("%d\n", events.count);
    da_append(&events, *notey);
  }
  // #endif

  // Event event = {NoteOn, 4};
  // Event event2 = {NoteOn, 10};

  da_append(&events, note1);

  create_event(tempoEvent2, SetTempo, 0, 120);
  da_append(&events, tempoEvent2);

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
void read_events(FILE *file, size_t offset, EventList *eventList) {

  u_int32_t count = 0;
  fread(&count, sizeof(u_int32_t), 1, file);
  // printf("Count: %d\n", count);
  while (eventList->count < count) {
    if (feof(file))
      return;
    EventType type = 0;
    fread(&type, sizeof(EventType), 1, file);
    // printf("file: %d\n", feof(file));

    // printf("Type: %s\n", type_to_string(type));
    // printf("count: %d\n", eventList->count);

    void *data = malloc(size_of_event(type));

    fread(data, size_of_event(type), 1, file);

    Event event;
    event.type = type;
    event.data = data;

    // printf("tempo: %d\n", ((SetTempoEvent*)event.data)->tempo);
    // eventList->items[0] = event;
    // eventList->count++;
    // if (type == SetTempo) {
    //   printf("tempo: %d\n", ((SetTempoEvent *)(event.data))->tempo);
    // } else if (type == NoteOn) {
    //   printf("Note deltaTime: %d\n", ((NoteOnEvent
    //   *)(event.data))->deltaTime);
    // }
    da_append(eventList, event);
  }
}
// void read_events(char *buffer, size_t offset, EventList *eventList) {
//   u_int32_t count = 0;

//   if (eventList->count == 0) {
//     memcpy(&count, &buffer[offset], sizeof(u_int32_t));
//     offset += sizeof(u_int32_t);
//   } else {
//     count = eventList->count;
//     // printf("nahhh\n");
//   }

//   printf("event count: %d\n", count);

//   while (1) {
//     // if (offset >= 2048 - sizeof(EventType)) return;
//     EventType type;
//     memcpy(&type, &buffer[offset], sizeof(EventType));
//     // printf("Type: %x\n", type);
//     offset += sizeof(EventType);
//     if (offset >= 2048 - size_of_event(type)){
//       printf("offset: %zu\n", offset);
//       printf("el count: %u\n", eventList->count);

//       return;
//     }

//     void *data = malloc(size_of_event(type));

//     memcpy(data, &buffer[offset], size_of_event(type));

//     Event event;
//     event.type = type;
//     event.data = data;
//     // printf("tempo: %d\n", ((SetTempoEvent*)event.data)->tempo);
//     // eventList->items[0] = event;
//     // eventList->count++;
//     // if (type == SetTempo) {
//     //   printf("tempo: %d\n", ((SetTempoEvent *)(event.data))->tempo);
//     // } else if (type == NoteOn) {
//     //   printf("Note deltaTime: %d\n", ((NoteOnEvent
//     *)(event.data))->deltaTime);
//     // }
//     da_append(eventList, event);

//     offset += size_of_event(type);
//   }
//   printf("el count: %d", eventList->count);
//   // printf("data: %d\n", ((NoteOnEvent*)eventList->items[1].data)->noteID);
// }

int read_gtab() {
  // char buffer[2048];
  char buffer[4 + 6];

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

  

  EventList eventList = {0};
  // int end_of_file = feof(file);
  // do {
  //   read_events(buffer, i, &eventList);
  //   fread(buffer, sizeof(char), sizeof(buffer), file);
  //   i = 0;
  //   end_of_file = feof(file);
  // } while (!end_of_file);
  read_events(file, i, &eventList);

  GuitarTab *tab;

  tab = malloc(sizeof(GuitarTab));

  // tab->bars = bar;
  tab->header = header;
  tab->events = &eventList;

  #ifdef TEST_MODE
  print_events(tab);
  printf("Magic number valid\n");
  printf("Magic number: %.4s\n", magic);
  printf("the tuning is: ");
  for (size_t i = 0; i < sizeof(header.tuning); i++) {
    printf("%s ", note_enum_str(header.tuning[i]));
  }
  printf("\n");
  #endif
  return 0;
}

int main() {

  GTabHeader header = {MAGIC_BYTES, {E, A, D, G, B, E}};
  write_gtab(&header);
  read_gtab();
  return 0;
}