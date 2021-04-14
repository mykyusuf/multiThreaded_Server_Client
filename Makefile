all: tcpclient tcpserver multi-tcpserver async-tcpserver

tcpclient : tcpclient.c
	gcc -o tcpclient tcpclient.c


tcpserver : tcpserver.c
	gcc -pthread -o tcpserver tcpserver.c

multi-tcpserver : multi-tcpserver.c
	gcc -pthread -o multi-tcpserver multi-tcpserver.c 

async-tcpserver : async-tcpserver.c
	gcc -o async-tcpserver async-tcpserver.c

clean:
	rm -f *.o tcpclient tcpserver multi-tcpserver async-tcpserver
