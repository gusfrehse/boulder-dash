OBJS = jogo.o game_state.o input.o map.o stopwatch.o camera.o score.o game_over.o help.o texture.o

CFLAGS := -Wall -g
CFLAGS += $(shell pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --cflags)
LDFLAGS := -Wall -g
LDLIBS := $(shell pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --libs) -lm

all : jogo

jogo : $(OBJS)

jogo.o : jogo.c game_state.h input.h stopwatch.h camera.h game_over.h help.h
game_state.o : game_state.h input.h map.h camera.h score.h texture.h
input.o : input.h
map.o : map.h
camera.o : camera.h map.h
score.o : score.h
game_over.o : game_over.h game_state.h
help.o : help.h game_state.h
texture.o : texture.h map.h
stopwatch.o : stopwatch.h

clean : 
	-rm $(OBJS)

purge : clean
	-rm jogo
