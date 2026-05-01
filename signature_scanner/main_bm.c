#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned char signature[] = {0xDE, 0xAD, 0xEF};

#define TABLE_SIZE (UCHAR_MAX + 1)

void make_skip_table(size_t table[], const unsigned char pattern[],
                     size_t len) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    table[i] = len;
  }

  for (size_t i = 0; i < len - 1; i++) {
    table[pattern[i]] = len - 1 - i;
  }
}

int bm_search(const unsigned char text[], size_t text_len,
              const unsigned char pattern[], size_t pat_len) {
  if (pat_len == 0)
    return 0;
  if (pat_len > text_len)
    return -1;

  size_t skip_table[TABLE_SIZE];

  make_skip_table(skip_table, pattern, pat_len);

  size_t i = 0;
  unsigned char tail = pattern[pat_len - 1];
  while (i <= text_len - pat_len) {
    unsigned char c = text[i + pat_len - 1];
    if (c == tail) {
      if (pat_len == 1)
        return (int)i;
      size_t j = pat_len - 1;
      size_t k = i + pat_len - 1;
      while ((j > 0) && (pattern[j - 1] == text[k - 1])) {
        j--;
        k--;
      }
      if (j == 0)
        return (int)i;
    }
    i += skip_table[c];
  }
  return -1;
}

int main(int argc, char *argv[]) {
  FILE *fp;
  int fd;
  size_t file_size;
  unsigned char *buf;
  int check = -1;
  int result = 0;
  struct stat st;

  if (argc != 2) {
    perror("Error: arguments is invalid.\n");
    exit(1);
  }

  fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("Error: File open failed.\n");
  }

  if (fstat(fd, &st) == -1) {
    perror("Error: fstat.");
    close(fd);
    exit(1);
  }

  file_size = st.st_size;

  buf = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (buf == MAP_FAILED) {
    perror("Error: mmap");
    close(fd);
    exit(1);
  }

  close(fd);

  int pos = bm_search((const unsigned char *)buf, sizeof(buf) / sizeof(char),
                      (const unsigned char *)signature,
                      sizeof(signature) / sizeof(unsigned char));

  if (pos != -1) {
    printf("%d bytes matched.\n", pos);
  } else {
    printf("Not Matched.\n");
  }
  return 0;
}
