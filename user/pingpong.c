#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

int main() {
  int f2c[2];  // 父进程 -> 子进程的管道
  int c2f[2];  // 子进程 -> 父进程的管道

  pipe(f2c);  // 初始化管道1
  pipe(c2f);  // 初始化管道2

  int pid = fork();  // 创建子进程

  if (pid < 0) {
    // fork 错误
    printf("fork error\n");
    exit(1);
  } else if (pid == 0) {
    // 子进程
    close(f2c[1]);  // 关闭子进程写入p1的管道
    close(c2f[0]);  // 关闭子进程读取p2的管道

    int parent_pid;
    read(f2c[0], &parent_pid, sizeof(parent_pid));                    // 从管道p1读取父进程的PID
    printf("%d: received ping from pid %d\n", getpid(), parent_pid);  // 输出符合要求的格式

    int child_pid = getpid();                      // 获取子进程的真实PID
    write(c2f[1], &child_pid, sizeof(child_pid));  // 向管道p2写入子进程的真实PID
    close(f2c[0]);                                 // 关闭子进程读取p1的管道
    close(c2f[1]);                                 // 关闭子进程写入p2的管道
  } else {
    // 父进程
    close(f2c[0]);  // 关闭父进程读取p1的管道
    close(c2f[1]);  // 关闭父进程写入p2的管道

    int parent_pid = getpid();
    write(f2c[1], &parent_pid, sizeof(parent_pid));  // 向管道p1写入父进程的PID
    close(f2c[1]);                                   // 关闭父进程写入p1的管道

    int child_pid;
    read(c2f[0], &child_pid, sizeof(child_pid));                       // 从管道p2读取子进程的PID
    printf("%d: received pong from pid %d\n", parent_pid, child_pid);  // 输出符合要求的格式

    close(c2f[0]);  // 关闭父进程读取p2的管道

    // 等待子进程结束
    wait(0);
  }

  exit(0);
}
