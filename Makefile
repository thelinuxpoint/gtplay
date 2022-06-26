
CFLAGS:= -lasound
CC := g++


default: wav.o mp3.o
	@printf "Compiling main ... ";
	@$(CC) ./src/main.cpp  wav.o mp3.o $(CFLAGS);
	@printf "[OK]\n";

# main: wav.o 
# 	@if [ $(shell id -u) -ne 0 ]; then \
# 		echo "You need to run as root"; \
# 		exit; \
# 	else \
# 		printf "Compiling main ... ";\
# 		$(CC) ./src/main.cpp  wav.o $(CFLAGS);\
# 		printf "Done\n"; \
# 	fi

wav.o: ./src/wav.h ./src/wav.cpp
	@printf "Compiling wav.cpp ... ";
	@$(CC) -c ./src/wav.cpp -o wav.o $(CFLAGS)
	@printf "[OK]\n"

mp3.o: ./src/mp3.h ./src/mp3.cpp
	@printf "Compiling mp3.cpp ... ";
	@$(CC) -c ./src/mp3.cpp -o mp3.o $(CFLAGS)
	@printf "[OK]\n"


install:

config:
	@apt-get install libasound2-dev
