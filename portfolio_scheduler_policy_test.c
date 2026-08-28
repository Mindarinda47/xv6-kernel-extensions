#include "types.h"
#include "stat.h"
#include "user.h"

#define CHILDREN 3

static int
read_exact(int fd, char *buffer, int size)
{
  int total;
  int count;

  total = 0;
  while(total < size){
    count = read(fd, buffer + total, size - total);
    if(count <= 0)
      return -1;
    total += count;
  }
  return 0;
}

static int
run_case(int priorities[CHILDREN], char expected[CHILDREN])
{
  int gate[2];
  int result[2];
  int pids[CHILDREN];
  int i;
  char token;
  char actual[CHILDREN];

  if(pipe(gate) < 0 || pipe(result) < 0)
    return -1;

  for(i = 0; i < CHILDREN; i++){
    pids[i] = fork();
    if(pids[i] < 0)
      return -1;

    if(pids[i] == 0){
      close(gate[1]);
      close(result[0]);
      if(read(gate[0], &token, 1) != 1)
        exit();
      token = '0' + i;
      write(result[1], &token, 1);
      close(gate[0]);
      close(result[1]);
      exit();
    }

    if(setnice(pids[i], priorities[i]) != 0)
      return -1;
  }

  close(gate[0]);
  close(result[1]);
  token = 'x';
  for(i = 0; i < CHILDREN; i++)
    write(gate[1], &token, 1);
  close(gate[1]);

  setnice(getpid(), 39);
  if(read_exact(result[0], actual, CHILDREN) < 0)
    return -1;
  close(result[0]);
  for(i = 0; i < CHILDREN; i++)
    wait();
  setnice(getpid(), 20);

  for(i = 0; i < CHILDREN; i++)
    if(actual[i] != expected[i])
      return 0;
  return 1;
}

int
main(void)
{
  int priority_values[CHILDREN] = {30, 10, 20};
  int equal_values[CHILDREN] = {20, 20, 20};
  char priority_order[CHILDREN] = {'1', '2', '0'};
  char fcfs_order[CHILDREN] = {'0', '1', '2'};
  int priority_ok;
  int fcfs_ok;

  priority_ok = run_case(priority_values, priority_order);
  fcfs_ok = run_case(equal_values, fcfs_order);

  if(priority_ok == 1 && fcfs_ok == 1)
    printf(1, "scheduler policy test: PASS\n");
  else
    printf(1, "scheduler policy test: FAIL\n");

  exit();
}
