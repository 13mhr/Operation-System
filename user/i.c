void homework_wait() {
  pid_t pid[3];
  int i, child_status;
  for (i = 0; i < 3; i++) {
    if ((pid[i] = fork()) == 0) {
      exit(100 + i); /* Child */
    }
  }
  printf("hello!\n");
  for (i = 0; i < 3; i++) { /* Parent */
    pid_t wpid = wait(&child_status);
    if (WIFEXITED(child_status))
      printf("child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
    else
      printf("child %d terminate abnormally\n", wpid);
  }
}