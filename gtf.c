#include "gtf.h"

#include <stdio.h>
#define MAGIC {0x47, 0x54, 0x41, 0x42}

int write_gtab(GTabHeader *header) {

  FILE *file = fopen("mytab.gtab", "wb");

  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  printf("Header magic: %s\n", header->magic);
  size_t bytes_written = fwrite(header, sizeof(GTabHeader), 1, file);
  // bytes_written = fwrite(header->tuning, sizeof(header->tuning), 1, file);

  fclose(file);
  return 1;
}

int read_gtab() {
  FILE *file = fopen("mytab.gtab", "wb");
  
}

int main() { 
  printf("Hello!\n");

  GTabHeader header = {MAGIC, {E, A, D, G, B, E}};
  write_gtab(&header);
  return 0; 
}