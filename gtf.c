#include "gtf.h"
#include "basicz.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAGIC_BYTES \
  { 0x47, 0x54, 0x41, 0x42 }

#define END_BYTES \
  {0xFF, 0xF1, 0xFF, 0xF1}


const char* note_enum_str(int note) {
  switch (note)
  {
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

int write_gtab(GTabHeader *header) {
  FILE *file = fopen("mytab.gtab", "wb");

  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // Writes the header first
  printf("Header magic: %s\n", header->magic);
  size_t bytes_written = fwrite(header, sizeof(GTabHeader), 1, file);

  TimeSignature timeSig = {4, 4};
  EventList events = {0};
  Event event = {11, 0};
  Event event2 = {13, 10};

  da_append(&events, event);
  da_append(&events, event2);


  Bar bar = {120, timeSig, events};
  // EventList uses pointers so it must be unpacked first
  bytes_written = fwrite(&bar, sizeof(Bar) - sizeof(EventList), 1, file);
  fwrite(&events, sizeof(EventList) - sizeof(Event*), 1, file);
  fwrite(events.items, sizeof(Event), events.count, file);
  
  // for (int i = 0; i < events.count; i++) {
  // }

  // Unique code to specify the end of the file
  char end_bytes[4] = {0xFF, 0xF1, 0xFF, 0xF1};
  bytes_written = fwrite(end_bytes, sizeof(end_bytes), 1, file);


  fclose(file);
  return 1;
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

  memcpy(header.tuning, buffer + 4, 6 * sizeof(char));

    printf("the tuning is: ");
  for (size_t i = 0; i < 6; i++)
  {
    printf("%s ", note_enum_str(header.tuning[i]));
  }
  

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

}

int main() {
  printf("Hello!\n");

  GTabHeader header = {MAGIC_BYTES, {E, A, D, G, B, E}};
  write_gtab(&header);
  read_gtab();
  return 0;
}