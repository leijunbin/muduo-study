server:
	g++ util.cpp server.cpp ./net/Epoll.cpp ./net/Socket.cpp ./net/InetAddress.cpp ./net/Channel.cpp ./net/EventLoop.cpp ./net/Server.cpp ./net/Acceptor.cpp ./net/Connection.cpp ./net/Buffer.cpp -o server &&  \
	g++ util.cpp client.cpp -o client

clean:
	rm -rf server && rm -rf client