#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "SDL.h"

/* Global states */
SDL_Window* window;
SDL_Renderer* rnd;
SDL_AudioSpec audiofmt;

int col = 0;
int beep = 0;
double audiophase = 0.0;
int cnt;
enum phase_e {
    PHASE_INIT,
    PHASE_BLANK,
    PHASE_PULSE
} phase;

static void
render_audio(void* bogus, void* stm, int len){
    float* stream = (float *)stm;
    const int numsamples = (len / sizeof(float)) / 2;
    const double freq = (double)audiofmt.freq;
    const double step = (1000.0 / freq);
    const float s = beep ? 1.0 : 0.0;
    const float k = 0;
    float sample;

    /* Fill samples */
    for(int i = 0; i != numsamples; i++){
        if(audiophase > 0.5){
            sample = s;
        }else{
            sample = k;
        }
        audiophase += step;
        if(audiophase > 1.0){
            audiophase -= 1.0;
        }

        stream[i*2] = sample;
        stream[i*2+1] = sample;
    }
}

static void
loop(void* bogus){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            SDL_Quit();
            exit(0);
        }
        if(event.type == SDL_WINDOWEVENT){
            if(event.window.event == SDL_WINDOWEVENT_CLOSE){
                SDL_Quit();
                exit(0);
            }
        }
    }

    /* Logic */
    switch(phase){
        case PHASE_INIT:
            col = 0;
            beep = 1;
            cnt = 0;
            phase = PHASE_BLANK;
            break;

        case PHASE_BLANK:
            if(cnt == 30){
                cnt = 0;
                beep = 1;
                col = 1;
                phase = PHASE_PULSE;
            }else{
                cnt ++;
            }
            break;

        case PHASE_PULSE:
            if(cnt == 0){
                beep = 0;
                cnt = 1;
            }else{
                cnt = 0;
                col = 0;
                phase = PHASE_BLANK;
            }
            break;
    }

    /* Draw */
    if(col){
        SDL_SetRenderDrawColor(rnd, 255, 255, 255, 255);
    }else{
        SDL_SetRenderDrawColor(rnd, 0, 0, 0, 255);
    }
    SDL_RenderClear(rnd);

    SDL_RenderPresent(rnd);
}

int
SDL_main(int ac, char** av){
    SDL_AudioSpec want, have;
    SDL_DisplayMode mode;
    uint32_t flags,w,h;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)){
        return -1;
    }

    /* Init video */
    flags = SDL_WINDOW_RESIZABLE ;
    //flags = SDL_WINDOW_FULLSCREEN;

    if(SDL_GetCurrentDisplayMode(0, &mode)){
        return -1;
    }

    /*
    w = mode.w;
    h = mode.h;
    */
    w = 2000;
    h = 2000;

    window = SDL_CreateWindow("avdelay", 
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              w, h, flags);


    if(! window){
        return -1;
    }

    rnd = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    /* Init Audio */
    SDL_zero(want, sizeof(want));
    want.freq = 48000;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 0;
    want.callback = render_audio;
    if(SDL_OpenAudio(&want, &audiofmt)){
        return -1;
    }
    if(audiofmt.format != AUDIO_F32){
        return -1;
    }
    if(audiofmt.channels != 2){
        return -1;
    }

    SDL_PauseAudio(0);

    phase = PHASE_INIT;
    cnt = 0;
    while(1){
        loop(0);
    }

    return 0;
}
