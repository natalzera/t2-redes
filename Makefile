rs: server
	./server

ms:
	gcc server.c segment.h segment.c list.h list.c nick.h nick.c channel.h channel.c -o server -lpthread

rc: client
	./client

mc:
	gcc client.c segment.h segment.c nick.h nick.c -o client -lpthread