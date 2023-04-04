server:
	g++ server.cpp -o server &&  \
	g++ client.cpp -o client

clean:
	rm -rf server && rm -rf client

