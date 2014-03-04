# For BBPlayer

NAME = BBPlayer
CC = gcc
CFLAGS = -Wall -mwindows -mno-cygwin -Os \
         -I/usr/local/include/BB4W -L/usr/local/lib/BB4W
TARGETS = BBPlayer.dll for_lilith.dll for_winamp.dll for_foobar2000.dll
DOCUMENTS = LICENSE README




all: build

build: $(TARGETS)


%.dll: %.o
	$(CC) $(CFLAGS) -o $@ -shared \
		-Wl,--whole-archive $^ \
		-Wl,--no-whole-archive -lbb4w

BBPlayer.dll: BBPlayer.o
for_lilith.dll: for_lilith.o
for_winamp.dll: for_winamp.o
for_foobar2000.dll: for_foobar2000.o

BBPlayer.o: BBPlayer.cpp
	$(CC) $(CFLAGS) -o $@ -c $^


clean:
	rm -f $(TARGETS) *.o *~


strip: build
	for i in $(TARGETS); do strip --strip-all $$i; done


nop:
	true




both-dist:
	make DIST_EXCLUDE="*.o" \
	     DIST_TARGETS="build strip" \
	     DIST_NAME="$(NAME)-$(VERSION)" dist-proc
bin-dist:
	make DIST_EXCLUDE="*.c *.cpp *.h Makefile *.o" \
	     DIST_TARGETS="build strip" \
	     DIST_NAME="$(NAME)-$(VERSION)-bin" dist-proc
src-dist:
	make DIST_EXCLUDE="" \
	     DIST_TARGETS="nop" \
	     DIST_NAME="$(NAME)-$(VERSION)-src" dist-proc

dist-proc:
	if [ x"$(VERSION)" = x ]; then false; fi
	rm -rf $(DIST_NAME) $(DIST_NAME).tar.bz2
	cvs export -r `echo $(DIST_NAME) | sed -e 's/-[a-z]\+$$//;s/\./_/g'` \
	           -d $(DIST_NAME) prog/$(NAME)
	cd $(DIST_NAME) && make $(DIST_TARGETS)
	rm -rf `echo '$(DIST_EXCLUDE)' \
	          | sed -e 's!\([^ ]\+\)!$(DIST_NAME)/\1!g'`
	for document in $(DOCUMENTS); do \
		sed -e 's/$$/\r/' <$(DIST_NAME)/$$document \
		                  >$(DIST_NAME)/$$document.txt; \
		rm $(DIST_NAME)/$$document; \
	done
	tar jcvf $(DIST_NAME).tar.bz2 $(DIST_NAME)
	rm -rf $(DIST_NAME)


# __END__
