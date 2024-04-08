cc=g++
.PHONY:all
all:tcpserver tcpclient
tcpserver:tcpServer.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread
tcpclient:tcpClient.cc
	$(cc) -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -f tcpserver tcpclient
