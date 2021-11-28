OBJS = jogo.o game_state.o input.o map.o camera.o score.o game_over.o help.o texture.o sample.o easter_egg.o game_end.o

CFLAGS := -Wall -g
LDFLAGS := -Wall -g

ifdef TO_WINDOWS
	CC = x86_64-w64-mingw32-gcc
	CFLAGS += -I./build/allegro/include
	LDLIBS := $(shell pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --libs) -lm
	LDLIBS += -L./build/allegro/lib
else
	LDLIBS := $(shell pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --libs) -lm
	CFLAGS += $(shell pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --cflags)
endif


all : jogo

jogo : $(OBJS)

jogo.o : jogo.c game_state.h input.h camera.h game_over.h help.h easter_egg.h game_end.h
game_state.o : game_state.h input.h map.h camera.h score.h texture.h sample.h
input.o : input.h
map.o : map.h
camera.o : camera.h map.h
score.o : score.h
game_over.o : game_over.h game_state.h
help.o : help.h game_state.h
texture.o : texture.h map.h
sample.o : sample.h
easter_egg.o : easter_egg.h
game_end.o : game_end.h


clean : 
	-rm $(OBJS)

purge : clean
	-rm jogo
