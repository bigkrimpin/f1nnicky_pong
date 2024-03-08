CFLAGS= -std=c99 -Wall -Wno-switch -pedantic
LDFLAGS= -lm
LIBS=sdl2 SDL2_image SDL2_mixer

ifeq ($(RELEASE), true)
	CFLAGS += -O2 -s
	LDFLAGS += `pkg-config --libs --static $(LIBS)` -s -static
else
	CFLAGS += -g -O0
	LDFLAGS += `pkg-config --libs $(LIBS)`
endif

ifeq ($(WINDOWS), true)
	CC=x86_64-w64-mingw32-gcc
	CFLAGS += `x86_64-w64-mingw32-pkg-config --cflags --static $(LIBS)` -static -s -O2
	LDFLAGS= -lm `x86_64-w64-mingw32-pkg-config --libs --static $(LIBS)` -static
else
	CFLAGS += `pkg-config --cflags $(LIBS)`
endif

.PHONY: clean

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

f1nnicky_pong: main.o fontx2.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o f1nnicky_pong f1nnicky_pong.exe
