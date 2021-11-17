OBJS = jogo.o game_state.o input.o map.o stopwatch.o camera.o score.o

CFLAGS := -Wall -g
CFLAGS += $(shell pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 --cflags)
LDFLAGS := -Wall -g
LDLIBS := $(shell pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 --libs)

all : jogo

jogo : $(OBJS)

jogo.o : jogo.c game_state.h input.h stopwatch.h camera.h
game_state.o : game_state.h input.h map.h camera.h
input.o : input.h
map.o : map.h
camera.o : camera.h map.h
score.o : score.h
stopwatch.o : stopwatch.h

clean : 
	-rm $(OBJS)

purge : clean
	-rm jogo
