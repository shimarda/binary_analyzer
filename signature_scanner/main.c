#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned char signature[] = {0xDE, 0xAD, 0xEF};

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

  for (int i = 0; i < file_size - sizeof(signature) / sizeof(unsigned char);
       i++) {
    int res =
        memcmp(signature, (buf + i), sizeof(signature) / sizeof(unsigned char));

    if (res == 0) {
      printf("%d bytes : matched.\n", i);
      return 0;
    }
  }

  printf("signature not matched.\n");
  return 0;
}
