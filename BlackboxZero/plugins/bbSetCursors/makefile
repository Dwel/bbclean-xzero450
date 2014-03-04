# For bbSetCursors

NAME = bbSetCursors
CC = g++
CFLAGS = -Os -Wall -s
CPPFLAGS = -I../../blackbox -I. -mwindows -mno-cygwin 
LDFLAGS = $(CFLAGS)
TARGETS = bbSetCursors.dll SetCursors.exe
LIBSDLL = -gdi32 ../../blackbox/libBlackbox.a
LIBSEXE = -gdi32



all: build

build: $(TARGETS) 

bbSetCursors.dll: bbplugin.o stringutils.o updatecursors.o configfile.o registry.o
	$(CC) $(CFLAGS) -o $@ -shared $^ $(LIBSDLL)
	ls -al $@
	
SetCursors.exe: main.o stringutils.o updatecursors.o configfile.o registry.o	
	$(CC) $(CFLAGS) -o $@ $^ $(LIBSEXE)
	ls -al $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

clean:
	rm -f $(TARGETS) `find -name '*.o' -or -name '*~'`