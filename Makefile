CXX=g++
CXXFLAGS= -Wall -g -I./include

server: ./src/*.cpp
	$(CXX) -o ./bin/server  $^ $(CXXFLAGS) -lpthread -lmysqlclient

clean:
	rm  -r ./bin/server
