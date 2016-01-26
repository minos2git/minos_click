all: ptcpclient pudpclient ptcpserver pudpserver
ptcpclient:  tcpclient.cc
	gcc tcpclient.cc -o ptcpclient
pudpclient:    udpclient.cc
	gcc udpclient.cc -o pudpclient
ptcpserver:  tcpserver.cc
	gcc tcpserver.cc -o ptcpserver
pudpserver:    udpserver.cc
	gcc udpserver.cc -o pudpserver
clean:
	rm p*
