#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <limits.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#define SYSCALL(call, name) if (call == -1) { perror(name); return 1; }

#define PORT 42069
#define HOST "127.0.0.1"

void sigchld_handler(int s) {
  int saved = errno;
  while (wait(NULL) > 0);
  errno = saved;
}

int main(int argc, const char* argv[]) {
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  SYSCALL(sigaction(SIGCHLD, &sa, NULL), "server: sigaction");
  
  int status;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = inet_addr(HOST);
  
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  int yes = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  SYSCALL(bind(sockfd, (struct sockaddr*) &addr, sizeof addr), "server: bind");

  SYSCALL(listen(sockfd, 1000), "server: listen");
  printf("server: listening on port %d\n", PORT);

  int addrlen = sizeof(struct sockaddr_storage);
  const char* message = "hello, world\n";

  while (1) {
    struct sockaddr client_addr;
    int newsock = accept(sockfd, (struct sockaddr*) &client_addr, &addrlen);
    if (newsock == -1) {
      perror("server: accept");
      return 1;
    }
    
    char client_ip[1024];
    inet_ntop(AF_INET, &(((struct sockaddr_in*) &client_addr)->sin_addr),
              client_ip, 1024);
    printf("server: accepted connection from %s\n", client_ip);
      
    if (fork() == 0) {
      close(sockfd);
      
      for (int i = 0; i < 1000; i++) {
        SYSCALL(send(newsock, message, strlen(message), 0), "server: send");
      }
      
      printf("server: closing connection\n");
      close(newsock);
      exit(0);
    }
    close(newsock);
  }
    
  printf("server: cleaning up\n");
  close(sockfd);
}
