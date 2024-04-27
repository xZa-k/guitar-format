#include "gtf.h"
#include "basicz.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

ZString format_tab(GuitarTab tab) {}

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
  Event event = {10, 4};
  Event event2 = {13, 10};

  da_append(&events, event);
  da_append(&events, event2);
  events.capacity += 512;

  Bar bar = {timeSig, 120, events};
  // EventList uses pointers so it must be unpacked first
  size_t bar_unpacked_size = sizeof(Bar) - sizeof(EventList);
  bytes_written = fwrite(&bar, bar_unpacked_size, 1, file);
  bytes_written = fwrite(&bar.events, sizeof(u_int32_t) * 2, 1, file);

  fwrite(events.items, sizeof(Event), events.count, file);

  // for (int i = 0; i < events.count; i++) {
  // }

  // Unique code to specify the end of the file
  char end_bytes[4] = {0xFF, 0xF1, 0xFF, 0xF1};
  bytes_written = fwrite(end_bytes, sizeof(end_bytes), 1, file);

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

Bar *read_bar(char *buffer, size_t i) {
  Bar* bar;
  bar = malloc(sizeof(Bar));

  memcpy(&bar->timeSignature, &buffer[i], sizeof(bar->timeSignature));
  i += sizeof(bar->timeSignature);
  memcpy(&bar->bpm, &buffer[i], sizeof(bar->bpm));
  i += sizeof(bar->bpm);

  printf("BPM: %d\n", bar->bpm);
  printf("Time signature: %d/%d\n", bar->timeSignature.numerator,
         bar->timeSignature.denominator);

  printf("i: %zu\n", i);
  // EventList eventList = {0};
  // bar.events = eventList;

  memcpy(&bar->events.count, &buffer[i], sizeof(u_int32_t));
  printf("Event count: %u\n", bar->events.count);
  i += sizeof(u_int32_t);

  memcpy(&bar->events.capacity, &buffer[i], sizeof(u_int32_t));
  printf("Capacity count: %u\n", bar->events.capacity);
  i += sizeof(u_int32_t);

  bar->events.items = malloc(bar->events.capacity);

  for (size_t e = 0; e < bar->events.count; e++) {
    Event event = {0};
    memcpy(&event, &buffer[i + (e * sizeof(Event))], sizeof(Event));
    bar->events.items[e] = event;
  }
  for (size_t j = 0; j < bar->events.count; j++) {
    printf("note: %d, time %d\n", bar->events.items[j].noteID,
           bar->events.items[j].deltaTime);
  }

  return bar;
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

  read_bar(&buffer, i);
  // while (!end) {
  //   printf("%c\n", buffer[i]);

  //   if (!memcmp(&buffer[i], end_bytes, sizeof(end_bytes))) {
  //     printf("end of\n");
  //     end = 1;
  //   }
  //   else {
  //     i++;

  //   }
  // }
  return 0;
}

int main() {
  printf("Hello!\n");

  GTabHeader header = {MAGIC_BYTES, {E, A, D, G, B, E}};
  write_gtab(&header);
  read_gtab();
  return 0;
}