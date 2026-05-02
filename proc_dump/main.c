#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void dump_mem(int pid, unsigned long start_addr, unsigned long end_addr) {
  int fd;
  char path[256];
  sprintf(path, "/proc/%d/mem", pid);
  size_t read_size;

  fd = open(path, O_RDONLY);
  if (fd == -1) {
    perror("Error: mem File open failed.");
    exit(1);
  }

  lseek(fd, start_addr, SEEK_SET);

  char *mem_data = malloc(end_addr - start_addr);
  if (mem_data == NULL) {
    perror("Error: malloc failed");
    exit(1);
  }

  read_size = read(fd, mem_data, end_addr - start_addr);
  if (read_size == -1) {
    perror("Error: read failed");
    exit(1);
  }

  int fd_out = open("dump.bin", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd_out == -1) {
    perror("Error: output file open failed");
    exit(1);
  }

  int s = write(fd_out, mem_data, read_size);

  close(fd);
  close(fd_out);
  free(mem_data);
}

int main(int argc, char *argv[]) {
  int pid;
  FILE *fp;
  char path[256];
  unsigned long start_addr, end_addr;

  if (argc != 2) {
    perror("Error: argument invalid\n");
    exit(1);
  }

  pid = atoi(argv[1]);

  ptrace(PTRACE_ATTACH, pid, NULL, NULL);

  sprintf(path, "/proc/%d/maps", pid);

  fp = fopen(path, "rb");
  if (fp == NULL) {
    perror("Error: open failed.");
    exit(1);
  }

  char line[256];
  char perms[5];
  while (fgets(line, sizeof(line), fp) != NULL) {
    sscanf(line, "%lx-%lx %s\n", &start_addr, &end_addr, perms);
    if (memcmp(perms, "r-xp", 4) == 0) {
      dump_mem(pid, start_addr, end_addr);
      break;
    }
  }
  ptrace(PTRACE_DETACH, pid, NULL, NULL);
  return 0;
}
