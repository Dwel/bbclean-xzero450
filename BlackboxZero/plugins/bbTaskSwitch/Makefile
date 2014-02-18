# For BBTaskSwitch

NAME = BBTaskSwitch
CC = gcc
CFLAGS = -Os -Wall -s
CPPFLAGS = -I../../blackbox -I. -mwindows -mno-cygwin 
LDFLAGS = $(CFLAGS)
TARGETS = BBTaskSwitch.dll ts.dll ts.lib mod_nix.dll
DOCUMENTS = README
CONFS = BBTaskSwitch.rc
LIBS = ../../blackbox/libBlackbox.a



all: build

build: $(TARGETS)

BBTaskSwitch.dll: BBTaskSwitch.o cpp_adaptor.o misc.o

ts.lib: ts.dll

ts.dll: ts.o
	$(CC) $(CFLAGS) -o $@ -shared \
		-Wl,--out-implib=ts.lib \
		$^ $(LIBS) -lgdi32
	ls -al ts.dll

mod_nix.dll: mod_nix.o
	$(CC) $(CFLAGS) -o $@ -shared \
		$^ $(LIBS) -lgdi32
	ls -al mod_nix.dll

%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

%.dll: %.o
	$(CC) $(CFLAGS) -o $@ -shared \
		 $^ $(LIBS)
	ls -al $@

clean:
	rm -f $(TARGETS) `find -name '*.o' -or -name '*~'`


strip: build
	for i in $(TARGETS); do strip --strip-all $$i; done


nop:
	true




both-dist:
	make --silent \
	     DIST_EXCLUDE="*.o" \
	     DIST_TARGETS="build strip" \
	     DIST_NAME="$(NAME)-$(VERSION)" dist-proc
bin-dist:
	make --silent \
	     DIST_EXCLUDE="src Makefile" \
	     DIST_TARGETS="build strip" \
	     DIST_NAME="$(NAME)-$(VERSION)-bin" dist-proc
src-dist:
	make --silent \
	     DIST_EXCLUDE="" \
	     DIST_TARGETS="nop" \
	     DIST_NAME="$(NAME)-$(VERSION)-src" dist-proc

dist-proc:
	echo; echo dist-proc: Check VERSION
	if [ x"$(VERSION)" = x ]; then false; fi
	echo; echo dist-proc: Remove garbages
	rm -rf $(DIST_NAME) $(DIST_NAME).tar.bz2
	echo; echo dist-proc: Export $(DIST_NAME)
	cvs export -r `echo $(DIST_NAME) | sed -e 's/-[a-z]\+$$//;s/\./_/g'` \
	           -d $(DIST_NAME) prog/$(NAME)
	echo; echo dist-proc: Do make
	cd $(DIST_NAME) && make $(DIST_TARGETS)
	echo; echo dist-proc: Remove files specified by '$(DIST_EXCLUDE)'
	if [ x"$(DIST_EXCLUDE)" != x ]; then \
	  find $(DIST_NAME) \
	    `echo '$(DIST_EXCLUDE)' \
	    | sed -e 's!\([^ ]\+\)!-or -name \1!g;s!^-or !!;s!^!( !;s!$$! )!'`\
	      >$(DIST_NAME)/.$(NAME).dist-proc.$$; \
	  cat $(DIST_NAME)/.$(NAME).dist-proc.$$; \
	  rm -rf `cat $(DIST_NAME)/.$(NAME).dist-proc.$$`; \
	  rm -f $(DIST_NAME)/.$(NAME).dist-proc.$$; \
	fi
	echo; echo dist-proc: Convert LF/CRLF and rename documents
	for document in $(DOCUMENTS); do \
		unix2dos <$(DIST_NAME)/$$document \
		         >$(DIST_NAME)/$$document.txt; \
		rm $(DIST_NAME)/$$document; \
	done
	for conf in $(CONFS); do \
		unix2dos $(DIST_NAME)/$$conf; \
	done
	echo; echo dist-proc: Make bzip2-ed tarball
	tar jcvf $(DIST_NAME).tar.bz2 $(DIST_NAME)
	echo; echo dist-proc: Remove a garbege
	rm -rf $(DIST_NAME)




# __END__
