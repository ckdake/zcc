CFLAGS = 
LIBS = -lcrypto


all: DHClient DHServer

DHClient: 
	g++ $(CFLAGS) -o DHClient DHClient.cpp DieWithError.c $(LIBS)

DHServer:
	g++ $(CFLAGS) -o DHServer DHServer.c HandleTCPClient.cpp DieWithError.c $(LIBS)

clean:
	rm -rf DHClient DHServer
