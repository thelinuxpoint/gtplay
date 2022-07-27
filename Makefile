
CFLAGS:= -lasound -pthread
CC := g++

libs    := $(shell pkg-config --libs gtkmm-3.0)
libs    += $(shell pkg-config --libs glibmm-2.4)
libs    += $(shell pkg-config --libs giomm-2.4)
libs    += $(shell pkg-config --libs gtksourceviewmm-3.0)
# 
CFLAGS  += $(shell pkg-config --cflags gtkmm-3.0)
CFLAGS  += $(shell pkg-config --cflags glibmm-2.4)
CFLAGS  += $(shell pkg-config --cflags giomm-2.4)
CFLAGS  += $(shell pkg-config --cflags gtksourceviewmm-3.0)


default: wav.o gmp3.o id3.o gutil.o ui.o window.o gcpanel.o
	@printf "\033[1m\033[32mCompiling Target\033[0m \033[1m\033[36mgtplay\033[0m v0.1.0 → ";
	@ar rcs libgtplay.a wav.o gmp3.o id3.o gutil.o ui.o window.o gcpanel.o
	@$(CC) ./src/main.cpp -o gtplay -L . -lgtplay $(CFLAGS) $(libs);
	@printf "[\033[1m\033[35mOK\033[0m]\n";

gutil.o: ./src/util/gutil.hpp ./src/util/gutil.cpp
	@printf "\033[1m\033[32mBuilding Object File\033[0m gutil.cpp ⇒ gutil.o → ";
	@$(CC) -c  ./src/util/gutil.cpp -o gutil.o $(CFLAGS)
	@printf "[\033[1m\033[35mOK\033[0m]\n"

wav.o: ./src/gwav/wav.h ./src/gwav/wav.cpp
	@printf "\033[1m\033[32mBuilding Object File\033[0m wav.cpp ⇒ wav.o → ";
	@$(CC) -c  ./src/gwav/wav.cpp -o wav.o $(CFLAGS)
	@printf "[\033[1m\033[35mOK\033[0m]\n"

gmp3.o: ./src/gmp3/gmp3.hpp ./src/gmp3/gmp3.cpp
	@printf "\033[1m\033[32mBuilding Object File\033[0m gmp3.cpp ⇒ gmp3.o → ";
	@$(CC) -c  ./src/gmp3/gmp3.cpp -o gmp3.o $(CFLAGS)
	@printf "[\033[1m\033[35mOK\033[0m]\n"

id3.o: ./src/util/id3.h ./src/util/id3.cpp
	@printf "\033[1m\033[32mBuilding Object File\033[0m id3.cpp ⇒ id3.o → ";
	@$(CC) -c ./src/util/id3.cpp -o id3.o  $(CFLAGS)
	@printf "[\033[1m\033[35mOK\033[0m]\n"

ui.o: ./src/ui/ui.hpp ./src/ui/ui.cpp
	@printf "\033[1m\033[32mBuilding Object File\033[0m ui.cpp ⇒ ui.o → ";
	@$(CC) -c -std=c++20 -fmodules-ts ./src/ui/ui.cpp -o ui.o  $(CFLAGS)
	@printf "[\033[1m\033[35mOK\033[0m]\n"

window.o: ./src/ui/window.hpp ./src/ui/window.cpp
	@printf "\033[1m\033[32mBuilding Object File\033[0m window.cpp ⇒ window.o → ";
	@$(CC) -c ./src/ui/window.cpp -o window.o  $(CFLAGS)
	@printf "[\033[1m\033[35mOK\033[0m]\n"

gcpanel.o: ./src/ui/gcpanel.hpp ./src/ui/gcpanel.cpp
	@printf "\033[1m\033[32mBuilding Object File\033[0m gcpanel.cpp ⇒ gcpanel.o → ";
	@$(CC) -c ./src/ui/gcpanel.cpp -o gcpanel.o  $(CFLAGS)
	@printf "[\033[1m\033[35mOK\033[0m]\n"

install:
	@echo "Not Configured";

config:
	@apt-get install libasound2-dev

run:
	@make
	@./gtplay
