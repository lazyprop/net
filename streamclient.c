#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <limits.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define PORT 42069
#define HOST "127.0.0.1"

int main() {
  int status;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = inet_addr(HOST);

  int addrlen = sizeof(struct sockaddr_storage);
  
  printf("trying to connect to port %d\n", PORT);
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (connect(sockfd, (struct sockaddr*) &addr, addrlen) == -1) {
    perror("connect");
    return 1;
  }
  printf("connection established\n");

  char buff[1024];
  long long total_size = 0;
  while (1) {
    memset(buff, 0, sizeof(buff));
    int msg_size;
    if ((msg_size = recv(sockfd, buff, 1024, 0)) == -1) {
      perror("recv");
      return 1;
    }
    if (msg_size == 0) break;

    total_size += msg_size;
    printf("received message of length %d\n", msg_size);
    printf("%s\n", buff);
  }

  printf("received total data: %lld\n", total_size);
    
  printf("closing sockets\n");
  close(sockfd);
}
