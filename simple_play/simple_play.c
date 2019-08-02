#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <zffqueue.h>
#include <string.h>
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
	if(data == NULL)
		return 0; 
    	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    	SDL_RenderClear(renderer);

        SDL_UpdateYUVTexture(texture, NULL, data, linesz, test_data, test_linesz, test_data, test_linesz);

	zffqueue_pop();

    	SDL_RenderCopy(renderer, texture, NULL, NULL);
    	SDL_RenderPresent(renderer);
	return 0;
}

int main(){
    	int flags;
	int ret;
	ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if(ret != 0){
		printf("SDL init error\n");
		exit(1);
	}
	flags = SDL_INIT_VIDEO|SDL_WINDOW_BORDERLESS;
        window = SDL_CreateWindow("simple_play", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, flags);
	if(!window){
		printf("window error\n");
		exit(1);
	}
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!renderer){
		printf("render error\n");
		exit(1);
	}
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12/*SDL_PIXELFORMAT_IYUV*/, SDL_TEXTUREACCESS_STREAMING, 640, 480);
	if(!texture){
		printf("texture error\n");
		exit(1);
	}
	signal(SIGTSTP , sigterm_handlerz2);
	signal(SIGINT , sigterm_handlerz2);
	signal(SIGQUIT , sigterm_handlerz2);
	signal(SIGHUP , sigterm_handlerz2);
	signal(SIGTERM , sigterm_handlerz2);

	zffqueue_map();
	test_data = malloc(zffqueue_get_onesz()/4);
	test_linesz = zffqueue_get_linesz()/2; 
	memset(test_data, 128, zffqueue_get_onesz()/4);

	while(1){
		if(play() < 0){
			break;
		}
	}


        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    	SDL_Quit();
	
	return 0;
}
