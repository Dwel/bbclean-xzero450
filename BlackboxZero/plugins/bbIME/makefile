# --------------------------------------------------------------------
# makefile for bbIME - gcc mingw


# -------------------------------------------
# targets

all: bbime.dll

# -------------------------------------------

clean:
	rm -f bbime.o bbime.dll

# --------------------------------------------------------------------
CC      = gcc
CFLAGS  = -Os -Wall -c -mwindows -I../../blackbox
LIBS    = -lshell32 -luser32 -lkernel32 -lgdi32 -lcomctl32 -lstdc++ ../../blackbox/libBlackbox.a

# --------------------------------------------------------------------
bbime.o: bbime.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

bbime.dll: bbime.o
	$(CC) -shared -s -o $@ $^ $(LIBS)
	ls -al bbime.dll


# --------------------------------------------------------------------
