#!gmake
#
# $Header: /home/grantham/cvsroot/projects/modules/tc/Makefile.release,v 1.1 2000/09/21 19:35:44 grantham Exp $
#

LIBDEST	=	/usr/lib/
INCDEST	=	/usr/include/ac/

# Use -DDEBUG to turn on messages on warnings and errors in
# the library.  Add -DINFO to get internal runtime dumps.
# DEBUG=-DDEBUG

# Preferred Linux $(CFLAGS) so far:
# CFLAGS="-Wall -O6 -malign-loops=4 -malign-jumps=4" $(DEBUG)
#
# More portable definitions
CFLAGS=-g $(DEBUG)

# Make sure to bring in math library
LDFLAGS=-lm

# These objects are part of the library
#
OBJECTS=tc.o

# On some systems (e.g. Irix 5 and 6) there are suffixes for different
# object styles (e.g. 32 vs 64).  You can set LIBSUF under Irix as
# long as you also edit CFLAGS to match.
#
LIBNAME=actc
LIBSUF=
ARNAME=lib$(LIBNAME)$(LIBSUF).a

# Sample programs and demos
#
PROGS=tctest tctest2 tcsample 
# tcview

all: $(PROGS)

$(PROGS):: $(ARNAME)

$(ARNAME): $(OBJECTS)
	$(AR) -rv $(ARNAME) $(OBJECTS)

clean:
	rm -rf *.so *.a *.o $(PROGS)

install:
	mkdir -p $(LIBDEST)
	cp $(ARNAME) $(LIBDEST)
	mkdir -p $(INCDEST)
	cp tc.h $(LIBDEST)

# Interactive viewer demo that I have not yet written
#
# tcview: tc.c tcview.c
# 	$(CC) $(CFLAGS) $(DEBUG) tc.c tcsample.c -o tcsample -L/usr/X11R6/lib -lglut -lGL -lGLU -lX11 -lXext -lXi -lXmu -lm

# vi:tabstop=8
#
