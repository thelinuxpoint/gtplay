
CFLAGS:= -lasound -pthread
CC := g++

libs    := $(shell pkg-config --libs gtkmm-3.0)
libs    += $(shell pkg-config --libs glibmm-2.4)
libs    += $(shell pkg-config --libs giomm-2.4)
libs    += $(shell pkg-config --libs gtksourceviewmm-3.0)


CFLAGS  += $(shell pkg-config --cflags gtkmm-3.0)
CFLAGS  += $(shell pkg-config --cflags glibmm-2.4)
CFLAGS  += $(shell pkg-config --cflags giomm-2.4)
CFLAGS  += $(shell pkg-config --cflags gtksourceviewmm-3.0)


default: wav.o gmp3.o id3.o gutil.o ui.o window.o
	@printf "Compiling gtplay v0.1.0 => ";
	@ar rcs libgtplay.a wav.o gmp3.o id3.o gutil.o ui.o window.o
	@$(CC) ./src/main.cpp -o gtplay -L . -lgtplay $(CFLAGS) $(libs);
	@printf "[OK]\n";

gutil.o: ./src/util/gutil.hpp ./src/util/gutil.cpp
	@printf "Building Object File [ gutil.cpp ~> gutil.o ] => ";
	@$(CC) -c  ./src/util/gutil.cpp -o gutil.o $(CFLAGS)
	@printf "[OK]\n"

wav.o: ./src/gwav/wav.h ./src/gwav/wav.cpp
	@printf "Building Object File [ wav.cpp ~> wav.o ] => ";
	@$(CC) -c  ./src/gwav/wav.cpp -o wav.o $(CFLAGS)
	@printf "[OK]\n"

gmp3.o: ./src/gmp3/gmp3.hpp ./src/gmp3/gmp3.cpp
	@printf "Building Object File [ gmp3.cpp ~> gmp3.o ] => ";
	@$(CC) -c  ./src/gmp3/gmp3.cpp -o gmp3.o $(CFLAGS)
	@printf "[OK]\n"

id3.o: ./src/util/id3.h ./src/util/id3.cpp
	@printf "Building Object File [ id3.cpp ~> id3.o ] => ";
	@$(CC) -c ./src/util/id3.cpp -o id3.o  $(CFLAGS)
	@printf "[OK]\n"

ui.o: ./src/ui/ui.hpp ./src/ui/ui.cpp
	@printf "Building Object File [ ui.cpp ~> ui.o ] => ";
	@$(CC) -c ./src/ui/ui.cpp -o ui.o  $(CFLAGS)
	@printf "[OK]\n"

window.o: ./src/ui/window.hpp ./src/ui/window.cpp
	@printf "Building Object File [ window.cpp ~> window.o ] => ";
	@$(CC) -c ./src/ui/window.cpp -o window.o  $(CFLAGS)
	@printf "[OK]\n"

install:
	@echo "Not Reached Max Effeciency";

config:
	@apt-get install libasound2-dev
