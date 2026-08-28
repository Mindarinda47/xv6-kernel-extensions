#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int result[2];
  int child_pid;
  int inherited_nice;
  int self_pid;
  int failures;

  failures = 0;
  self_pid = getpid();

  if(setnice(self_pid, 12) != 0 || getnice(self_pid) != 12)
    failures++;
  if(setnice(self_pid, -1) != -1)
    failures++;
  if(setnice(self_pid, 40) != -1)
    failures++;
  if(getnice(999999) != -1)
    failures++;

  if(pipe(result) < 0){
    printf(1, "scheduler API test: pipe failed\n");
    exit();
  }

  child_pid = fork();
  if(child_pid < 0){
    printf(1, "scheduler API test: fork failed\n");
    exit();
  }

  if(child_pid == 0){
    close(result[0]);
    inherited_nice = getnice(getpid());
    write(result[1], &inherited_nice, sizeof(inherited_nice));
    close(result[1]);
    exit();
  }

  close(result[1]);
  inherited_nice = -1;
  read(result[0], &inherited_nice, sizeof(inherited_nice));
  close(result[0]);
  wait();

  if(inherited_nice != 12)
    failures++;

  setnice(self_pid, 20);

  if(failures == 0)
    printf(1, "scheduler API test: PASS\n");
  else
    printf(1, "scheduler API test: FAIL (%d checks)\n", failures);

  exit();
}
