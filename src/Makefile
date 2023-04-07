server:
	g++ util.cpp server.cpp ./net/Epoll.cpp ./net/Socket.cpp ./net/InetAddress.cpp -o server &&  \
	g++ util.cpp client.cpp ./net/Epoll.cpp ./net/Socket.cpp ./net/InetAddress.cpp -o client

clean:
	rm -rf server && rm -rf client
