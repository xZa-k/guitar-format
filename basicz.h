#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define swap(a, b)                                                             \
  do {                                                                         \
    typeof(a) c = a;                                                           \
    a = b;                                                                     \
    b = c;                                                                     \
  } while (0)

void memcpy_with_endianness(void *dest, const void *src, size_t size) {
    const uint8_t *src_ptr = (const uint8_t *)src;
    uint8_t *dest_ptr = (uint8_t *)dest;

    // Copy bytes in reverse order to perform endianness conversion
    for (size_t i = 0; i < size; ++i) {
        dest_ptr[size - 1 - i] = src_ptr[i];
    }
}

typedef struct {
  char *items;
  size_t count;
  size_t capacity;
} ZString;

#define da_print(arr, fmt, ...)                                                \
  do {                                                                         \
    for (size_t i = 0; i < (arr)->count; i++) {                                \
      printf(fmt##__VA_ARGS__, (arr)->items[i]);                               \
    }                                                                          \
  } while (0)

#define DA_START_SIZE 256

#define da_pop(arr)                                                            \
  do {                                                                         \
    (arr)->items[(arr)->count] = 0;                                            \
    (arr)->count--;                                                            \
  } while (0)

#define da_append(arr, item)                                                   \
  do {                                                                         \
    if ((arr)->count >= (arr)->capacity) {                                     \
      (arr)->capacity = (arr)->capacity == 0                                   \
                            ? DA_START_SIZE                                    \
                            : (size_t)((arr)->capacity * 1.5);                 \
      (arr)->items =                                                           \
          realloc((arr)->items, (arr)->capacity * sizeof(*(arr)->items));      \
      assert((arr)->items != NULL && "Buy more RAM");                          \
    }                                                                          \
    (arr)->items[((arr)->count)++] = (item);                                   \
  } while (0)

#define da_bubble_reverse(arr)                                                 \
  do {                                                                         \
    int sorted = 0;                                                            \
    while (!sorted) {                                                          \
      sorted = 1;                                                              \
      for (size_t i = 0; i < (arr)->count - 1; i++) {                          \
        if ((arr)->items[i] < (arr)->items[i + 1]) {                           \
          swap((arr)->items[i], (arr)->items[i + 1]);                          \
          sorted = 0;                                                          \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

#define da_bubble(arr)                                                         \
  do {                                                                         \
    int sorted = 0;                                                            \
    while (!sorted) {                                                          \
      sorted = 1;                                                              \
      for (size_t i = 0; i < (arr)->count - 1; i++) {                          \
        if ((arr)->items[i] > (arr)->items[i + 1]) {                           \
          swap((arr)->items[i], (arr)->items[i + 1]);                          \
          sorted = 0;                                                          \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)
// quicksort fail
// #define da_sort(arr) \
//   do { \
//     typeof(*(arr)) tmp = {0}; \
//     tmp.count = (arr)->count; \
//     tmp.capacity = (arr)->capacity; \
//     tmp.items = malloc((arr)->capacity); \
//     memcpy(tmp.items, (arr)->items, (arr)->capacity); \
//     if ((arr)->count <= 1) \
//       break; \
//     for (size_t p = 0; p < (arr)->count; p++) { \
//       for (size_t i = p; i < (arr)->count; i++) { \
//         if (p == i) \
//           continue; \
//         swap((arr)->items[p], (arr)->items[i]); \
//       } \
//     } \
//   } while (0)

// #define da_append_many()

// #define da_append_str(arr, str) \
//     do { \
//         \
//     } while (0)
//
