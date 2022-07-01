
CFLAGS:= -lasound
CC := g++


default: wav.o gmp3.o id3.o gutil.o
	@printf "Compiling gtplay v0.1.0 => ";
	@ar rcs libgtplay.a wav.o gmp3.o id3.o gutil.o
	@$(CC) ./src/main.cpp -o gtplay -L . -lgtplay $(CFLAGS);
	@printf "[OK]\n";

gutil.o: ./src/util/gutil.hpp ./src/util/gutil.cpp
	@printf "Building Object File [ gutil.cpp ~> gutil.o ] => ";
	@$(CC) -c ./src/util/gutil.cpp -o gutil.o $(CFLAGS)
	@printf "[OK]\n"

wav.o: ./src/gwav/wav.h ./src/gwav/wav.cpp
	@printf "Building Object File [ wav.cpp ~> wav.o ] => ";
	@$(CC) -c ./src/gwav/wav.cpp -o wav.o $(CFLAGS)
	@printf "[OK]\n"

gmp3.o: ./src/gmp3/gmp3.hpp ./src/gmp3/gmp3.cpp gutil.o
	@printf "Building Object File [ gmp3.cpp ~> gmp3.o ] => ";
	@$(CC) -c ./src/gmp3/gmp3.cpp -o gmp3.o $(CFLAGS)
	@printf "[OK]\n"

id3.o: ./src/util/id3.h ./src/util/id3.cpp gutil.o
	@printf "Building Object File [ id3.cpp ~> id3.o ] => ";
	@$(CC) -c ./src/util/id3.cpp -o id3.o  $(CFLAGS)
	@printf "[OK]\n"

install:
	@echo "Not Configured yet";

config:
	@apt-get install libasound2-dev
