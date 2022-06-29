
CFLAGS:= -lasound
CC := g++


default: wav.o mp3.o id3.o
	@printf "Compiling main ... ";
	@$(CC) ./src/main.cpp  wav.o mp3.o id3.o $(CFLAGS);
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

mp3.o: ./src/gmp3/gmp3.hpp ./src/gmp3/gmp3.cpp
	@printf "Compiling mp3.cpp ... ";
	@$(CC) -c ./src/gmp3/gmp3.cpp -o mp3.o $(CFLAGS)
	@printf "[OK]\n"

id3.o: ./src/util/id3.h ./src/util/id3.cpp
	@printf "Compiling id3.cpp ... ";
	@$(CC) -c ./src/util/id3.cpp -o id3.o $(CFLAGS)
	@printf "[OK]\n"
install:

config:
	@apt-get install libasound2-dev
