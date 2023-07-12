rs: server
	./server

ms: server.c segment.h segment.c
	gcc server.c segment.h segment.c -o server

rc: client
	./client

mc: client.c segment.h segment.c
	gcc client.c segment.h segment.c -o client