CC=gcc -O3

stream: streamserver.c streamclient.c
	$(CC) streamserver.c -o streamserver
	$(CC) streamclient.c -o streamclient


run: stream
	./streamserver
