#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <zffqueue.h>
#include <string.h>
#include <unistd.h>
#include "libavutil/frame.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

static char * test_data;
static int test_linesz;

static void
sigterm_handlerz2(int sig)
{
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    	SDL_Quit();
	exit(0);	
}


static int play(){
	int ret;
	void *data = zffqueue_get();
	int linesz= zffqueue_get_linesz();
	//printf("sz %d\n", sz);
	if(data == NULL){
		return 0;
	} 
    	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    	SDL_RenderClear(renderer);

        SDL_UpdateYUVTexture(texture, NULL, data, linesz, test_data, test_linesz, test_data, test_linesz);

	zffqueue_pop();

    	SDL_RenderCopy(renderer, texture, NULL, NULL);
    	SDL_RenderPresent(renderer);
	return 1;
}

int main(){
    	int flags;
	int ret;

	zffqueue_map();
	int onesz = zffqueue_get_onesz();
	int linesz = zffqueue_get_linesz();
	int w = linesz;
	int h = onesz/linesz;
	printf("onesz %d, linesz %d, w %d, h%d\n", onesz, linesz, w, h);
	test_data = malloc(onesz/4);
	test_linesz = linesz/2; 
	memset(test_data, 128, onesz/4);

	ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if(ret != 0){
		printf("SDL init error\n");
		exit(1);
	}

	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	flags = SDL_INIT_VIDEO|SDL_WINDOW_BORDERLESS;
	//flags = SDL_INIT_VIDEO|SDL_WINDOW_RESIZABLE;
        window = SDL_CreateWindow("simple_play", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flags);
	if(!window){
		printf("window error\n");
		exit(1);
	}
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!renderer){
		printf("render error\n");
		exit(1);
	}
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12/*SDL_PIXELFORMAT_IYUV*/, SDL_TEXTUREACCESS_STREAMING, w, h);
	if(!texture){
		printf("texture error\n");
		exit(1);
	}
	signal(SIGTSTP , sigterm_handlerz2);
	signal(SIGINT , sigterm_handlerz2);
	signal(SIGQUIT , sigterm_handlerz2);
	signal(SIGHUP , sigterm_handlerz2);
	signal(SIGTERM , sigterm_handlerz2);

	SDL_Event event;

	while(1){
		SDL_PumpEvents();
		while (!SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
			if(play() == 0){//no data in quque, backoff
				nice(1);
			}else{
				nice(-2);//data in queue
			}
			SDL_PumpEvents();
		}
		
		switch (event.type) {
			case SDL_QUIT:
				printf("quit evnt\n");
				goto out;
			default:
				break;
		}
	}

out:
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    	SDL_Quit();
	
	return 0;
}
