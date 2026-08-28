#include "types.h"
#include "stat.h"
#include "user.h"

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

int
main(void)
{
  int gate[2];
  int result[2];
  int first_pid;
  int second_pid;
  int start;
  char token;
  char actual[2];

  if(pipe(gate) < 0 || pipe(result) < 0){
    printf(1, "nonpreemptive test: pipe failed\n");
    exit();
  }

  first_pid = fork();
  if(first_pid == 0){
    close(gate[1]);
    close(result[0]);
    read(gate[0], &token, 1);
    start = uptime();
    while(uptime() - start < 20)
      ;
    token = 'A';
    write(result[1], &token, 1);
    exit();
  }

  second_pid = fork();
  if(second_pid == 0){
    close(gate[1]);
    close(result[0]);
    read(gate[0], &token, 1);
    token = 'B';
    write(result[1], &token, 1);
    exit();
  }

  if(first_pid < 0 || second_pid < 0){
    printf(1, "nonpreemptive test: fork failed\n");
    exit();
  }

  setnice(first_pid, 20);
  setnice(second_pid, 20);
  close(gate[0]);
  close(result[1]);
  token = 'x';
  write(gate[1], &token, 1);
  write(gate[1], &token, 1);
  close(gate[1]);

  setnice(getpid(), 39);
  if(read_exact(result[0], actual, sizeof(actual)) < 0){
    printf(1, "nonpreemptive test: result read failed\n");
    exit();
  }
  close(result[0]);
  wait();
  wait();
  setnice(getpid(), 20);

  if(actual[0] == 'A' && actual[1] == 'B')
    printf(1, "nonpreemptive test: PASS\n");
  else
    printf(1, "nonpreemptive test: FAIL\n");

  exit();
}
