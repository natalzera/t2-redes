rs: server
	./server

ms:
	gcc server.c segment.h segment.c list.h list.c tables.h tables.c channel.h channel.c -o server -lpthread

rc: client
	./client

mc:
	gcc client.c segment.h segment.c tables.h tables.c -o client -lpthread