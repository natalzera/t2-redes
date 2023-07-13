rs: server
	./server

ms: server.c segment.h segment.c list.c list.h
	gcc server.c segment.h segment.c list.c list.h -o server

rc: client
	./client

mc: client.c segment.h segment.c
	gcc client.c segment.h segment.c -o client