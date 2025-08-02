all: app

app: ../../src/socket.cpp ../../src/logger.cpp ../../src/tcp.cpp main.cpp
	@g++ -std=c++23 -g -Wall -Wextra -pedantic \
		-I../../include \
		../../src/socket.cpp ../../src/logger.cpp ../../src/tcp.cpp main.cpp \
		-o app

.PHONY: clean run

clean:
	@rm -rf app

run: app
	@./app
	@rm -rf app
