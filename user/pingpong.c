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
    close(f2c[1]);  // 关闭子进程写入f2c的管道
    close(c2f[0]);  // 关闭子进程读取c2f的管道

    int parent_pid;
    char parent_char[5];         // 用于接收"ping"
    char child_char[] = "pong";  // 子进程要发送的消息

    read(f2c[0], &parent_pid, sizeof(parent_pid));                               // 从管道f2c读取父进程的PID
    read(f2c[0], parent_char, sizeof(parent_char));                              // 从管道读取"ping"
    printf("%d: received %s from pid %d\n", getpid(), parent_char, parent_pid);  // 输出符合要求的格式

    int child_pid = getpid();                       // 获取子进程的真实PID
    write(c2f[1], &child_pid, sizeof(child_pid));   // 向管道c2f写入子进程的真实PID
    write(c2f[1], child_char, sizeof(child_char));  // 向管道c2f写入"pong"
    close(f2c[0]);                                  // 关闭子进程读取f2c的管道
    close(c2f[1]);                                  // 关闭子进程写入c2f的管道
  } else {
    // 父进程
    close(f2c[0]);  // 关闭父进程读取f2c的管道
    close(c2f[1]);  // 关闭父进程写入c2f的管道

    int parent_pid = getpid();    // 获取父进程的PID
    char parent_char[] = "ping";  // 父进程发送的消息
    char child_char[5];           // 用于接收"pong"

    write(f2c[1], &parent_pid, sizeof(parent_pid));   // 向管道f2c写入父进程的PID
    write(f2c[1], parent_char, sizeof(parent_char));  // 向管道f2c写入"ping"
    close(f2c[1]);                                    // 关闭父进程写入f2c的管道

    int child_pid;
    read(c2f[0], &child_pid, sizeof(child_pid));                                 // 从管道c2f读取子进程的PID
    read(c2f[0], child_char, sizeof(child_char));                                // 从管道c2f读取"pong"
    printf("%d: received %s from pid %d\n", parent_pid, child_char, child_pid);  // 输出符合要求的格式

    close(c2f[0]);  // 关闭父进程读取c2f的管道

    // 等待子进程结束
    wait(0);
  }

  exit(0);
}
