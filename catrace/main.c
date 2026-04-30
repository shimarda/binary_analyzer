#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

void child(char *file) {

  char *args[] = {file, NULL};
  printf("exec : %s.\n", file);

  ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  execvp(file, args);
}

void output_trace() {}

int main(int argc, char *argv[]) {
  int fd;
  FILE *fp;
  int status;
  pid_t pid;
  int in_syscall = 0;

  if (argc != 2) {
    perror("Error: Argument fail.\n");
    exit(1);
  }

  pid = fork();
  if (pid == -1) {
    perror("Error: fork failed.\n");
    exit(1);
  }

  if (pid == 0) {
    child(argv[1]);
  }

  struct user_regs_struct regs;

  while (1) {
    pid = wait(&status);

    if (WIFEXITED(status) || WIFSIGNALED(status)) {
      break;
    }

    if (pid < 0) {
      perror("Error: wait.\n");
      exit(1);
    }

    if (WIFSTOPPED(status)) {
      if (in_syscall == 0) {

        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
        printf("Syscall: %llu\n", regs.orig_rax);
        in_syscall = 1;
      } else {
        in_syscall = 0;
      }
      ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    }
  }
}
