#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define DUMP(x)                                                                \
  do {                                                                         \
    printf("  %-12s = %llu(0x%llx)\n", #x, (unsigned long long)(x),            \
           (unsigned long long)(x));                                           \
  } while (0);

void dump_32bit(Elf32_Ehdr *ehdr) {
  DUMP(ehdr->e_type);
  DUMP(ehdr->e_machine);
  DUMP(ehdr->e_version);
  DUMP(ehdr->e_entry);
  DUMP(ehdr->e_phoff);
  DUMP(ehdr->e_shoff);
  DUMP(ehdr->e_flags);
  DUMP(ehdr->e_ehsize);
  DUMP(ehdr->e_phentsize);
  DUMP(ehdr->e_phnum);
  DUMP(ehdr->e_shentsize);
  DUMP(ehdr->e_shnum);
  DUMP(ehdr->e_shstrndx);
  printf("\n");
}

void dump_64bit(Elf64_Ehdr *ehdr) {

  DUMP(ehdr->e_type);
  DUMP(ehdr->e_machine);
  DUMP(ehdr->e_version);
  DUMP(ehdr->e_entry);
  DUMP(ehdr->e_phoff);
  DUMP(ehdr->e_shoff);
  DUMP(ehdr->e_flags);
  DUMP(ehdr->e_ehsize);
  DUMP(ehdr->e_phentsize);
  DUMP(ehdr->e_phnum);
  DUMP(ehdr->e_shentsize);
  DUMP(ehdr->e_shnum);
  DUMP(ehdr->e_shstrndx);
  printf("\n");
}

void dump_section_header_32(Elf32_Shdr *shdr) {
  DUMP(shdr->sh_name);
  DUMP(shdr->sh_type);
  DUMP(shdr->sh_flags);
  DUMP(shdr->sh_addr);
  DUMP(shdr->sh_offset);
  DUMP(shdr->sh_size);
  DUMP(shdr->sh_link);
  DUMP(shdr->sh_info);
  DUMP(shdr->sh_addralign);
  DUMP(shdr->sh_entsize);
  printf("\n");
}

void dump_section_header_64(Elf64_Shdr *shdr) {
  DUMP(shdr->sh_name);
  DUMP(shdr->sh_type);
  DUMP(shdr->sh_flags);
  DUMP(shdr->sh_addr);
  DUMP(shdr->sh_offset);
  DUMP(shdr->sh_size);
  DUMP(shdr->sh_link);
  DUMP(shdr->sh_info);
  DUMP(shdr->sh_addralign);
  DUMP(shdr->sh_entsize);
  printf("\n");
}

int main(int argc, char *argv[]) {
  int fd;
  FILE *file;
  Elf64_Ehdr *ehdr;
  struct stat stbuf;

  if (argc != 2) {
    printf("Error: Usage: ./static_analyzer <file name>\n");
    exit(1);
  }

  fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    printf("Error: open.\n");
    exit(1);
  }
  if ((file = fdopen(fd, "rb")) == NULL) {
    printf("Error: file open.\n");
    exit(1);
  }

  fstat(fd, &stbuf);
  unsigned char *buf = (unsigned char *)malloc(stbuf.st_size);
  if (buf == NULL) {
    printf("Error: Memory allocation failed.\n");
    fclose(file);
    exit(1);
  }

  fread(buf, 1, stbuf.st_size, file);
  fclose(file);

  ehdr = (Elf64_Ehdr *)buf;

  if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
    printf(" This file is not ELF.\n");
    exit(1);
  }

  for (int i = 0; i < EI_NIDENT; i++) {
    printf("%02x ", ehdr->e_ident[i]);
  }
  printf("\n");

  // 64bit or 32bit
  if (ehdr->e_ident[EI_CLASS] == ELFCLASS32) {
    Elf32_Ehdr *ehdr_32;
    if (stbuf.st_size >= sizeof(Elf32_Ehdr)) {

      dump_32bit(ehdr_32);

      Elf32_Shdr *shdr_array = (Elf32_Shdr *)(buf + ehdr_32->e_shoff);
      Elf32_Shdr *sh_strtab = &shdr_array[ehdr_32->e_shstrndx];
      char *strtab = (char *)(buf + sh_strtab->sh_offset);

      for (int i = 0; i < ehdr->e_shnum; i++) {
        printf("Name: %s\n", strtab + shdr_array[i].sh_name);
        dump_section_header_32(&shdr_array[i]);
      }
    } else {
      printf("Error: Truncated 32-bit ELF header.\n");
    }
  } else if (ehdr->e_ident[EI_CLASS] == ELFCLASS64) {
    if (stbuf.st_size >= sizeof(Elf64_Ehdr)) {

      dump_64bit(ehdr);

      Elf64_Shdr *shdr_array = (Elf64_Shdr *)(buf + ehdr->e_shoff);
      Elf64_Shdr *sh_strtab = &shdr_array[ehdr->e_shstrndx];
      char *strtab = (char *)(buf + sh_strtab->sh_offset);

      for (int i = 0; i < ehdr->e_shnum; i++) {
        printf("Name: %s\n", strtab + shdr_array[i].sh_name);
        dump_section_header_64(&shdr_array[i]);
      }
    } else {
      printf("Error: Truncated 64-bit ELF header.\n");
    }
  } else {
    printf("Format is invalid.\n");
    exit(1);
  }
  free(buf);
  return 0;
}
