#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int result[2];
  int parent_pid;
  int child_pid;
  int child_ok;

  if(pipe(result) < 0){
    printf(1, "getppid test: pipe failed\n");
    exit();
  }

  parent_pid = getpid();
  child_pid = fork();

  if(child_pid < 0){
    printf(1, "getppid test: fork failed\n");
    exit();
  }

  if(child_pid == 0){
    close(result[0]);
    child_ok = getppid() == parent_pid;
    write(result[1], &child_ok, sizeof(child_ok));
    close(result[1]);
    exit();
  }

  close(result[1]);
  child_ok = 0;
  read(result[0], &child_ok, sizeof(child_ok));
  close(result[0]);
  wait();

  if(child_ok)
    printf(1, "getppid test: PASS\n");
  else
    printf(1, "getppid test: FAIL\n");

  exit();
}
