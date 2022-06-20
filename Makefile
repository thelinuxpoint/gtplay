
CFLAGS:= -lasound
CC := g++

main: wav.o 
	@if [ $(shell id -u) -ne 0 ]; then \
		echo "You need to run as root"; \
		exit; \
	else \
		printf "Compiling main ... ";\
		$(CC) ./src/main.cpp  wav.o $(CFLAGS);\
		printf "Done\n"; \
	fi

wav.o: ./src/wav.h ./src/wav.cpp
	@printf "Compiling wav.cpp ... ";
	@$(CC) -c ./src/wav.cpp -o wav.o $(CFLAGS)
	@printf "Done\n"

mp3:


install:

config:
	@apt-get install libasound2-dev
