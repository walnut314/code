all: srv.out cli.out

srv.out: mux_tcpS.c tcpS.h utils_tcp.c
	gcc mux_tcpS.c utils_tcp.c -Wall -Os -o srv.out

cli.out: tcpC.c
	gcc tcpC.c -Wall -Os -o cli.out

clean:
	rm srv.out cli.out
