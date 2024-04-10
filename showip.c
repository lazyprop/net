#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: showip hostname\n");
    return 1;
  }

  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int status;
  if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 2;
  }

  printf("IP Addresses for %s\n", argv[1]);
  for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
    void* addr;
    char* ipver;
    if (p->ai_family == AF_INET) { // IPv4
      ipver = "IPv4";
      struct sockaddr_in* ipv4 = (struct sockaddr_in*) p->ai_addr;
      addr = &(ipv4->sin_addr);
    } else { // IPv6 // IPv4
      ipver = "IPv6";
      struct sockaddr_in6* ipv6 = (struct sockaddr_in6*) p->ai_addr;
      addr = &(ipv6->sin6_addr);
    }

    char ipstr[INET6_ADDRSTRLEN];
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    printf("%s: %s\n", ipver, ipstr);
  }

  freeaddrinfo(res);

}
