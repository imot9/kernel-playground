#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
  int fd;
  char c;

  fd = open("/dev/cdev0", O_RDWR);
  if (fd < 0) {
    perror("open");
  }

  while (read(fd, &c, 1)) {
    putchar(c);
  }

  close(fd);
  return 0;
}
