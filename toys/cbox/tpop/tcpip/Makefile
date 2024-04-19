all: srv.out cli.out

srv.out: thread_tcpS.c tcpS.h utils_tcp.c
	gcc thread_tcpS.c utils_tcp.c -Wall -Os -o srv.out

cli.out: tcpC.c
	gcc tcpC.c -Wall -Os -o cli.out

clean:
	rm srv.out cli.out
