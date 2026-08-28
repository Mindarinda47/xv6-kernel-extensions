#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"

int
main(void)
{
  int result[2];
  int child_pid;
  int child_ok;
  int failures;
  char *old_break;
  char *region;
  char *new_break;

  failures = 0;
  old_break = sbrk(0);
  region = sbrk(3 * PGSIZE);

  if(region != old_break)
    failures++;
  if(region[0] != 0 || region[PGSIZE] != 0 || region[2 * PGSIZE] != 0)
    failures++;

  region[0] = 11;
  region[PGSIZE] = 22;
  region[2 * PGSIZE] = 33;

  if(pipe(result) < 0){
    printf(1, "lazy allocation test: pipe failed\n");
    exit();
  }

  child_pid = fork();
  if(child_pid < 0){
    printf(1, "lazy allocation test: fork failed\n");
    exit();
  }

  if(child_pid == 0){
    close(result[0]);
    child_ok = region[0] == 11 &&
               region[PGSIZE] == 22 &&
               region[2 * PGSIZE] == 33;
    write(result[1], &child_ok, sizeof(child_ok));
    close(result[1]);
    exit();
  }

  close(result[1]);
  child_ok = 0;
  read(result[0], &child_ok, sizeof(child_ok));
  close(result[0]);
  wait();

  if(!child_ok)
    failures++;

  if(sbrk(-3 * PGSIZE) != old_break + (3 * PGSIZE))
    failures++;
  new_break = sbrk(0);
  if(new_break != old_break)
    failures++;

  if(failures == 0)
    printf(1, "lazy allocation test: PASS\n");
  else
    printf(1, "lazy allocation test: FAIL (%d checks)\n", failures);

  exit();
}
