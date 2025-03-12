#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define MAX_DEVICES   10

char devices[MAX_DEVICES][50];
int device_count = 0;

void scan_bluetooth_devices() {
    FILE *fp = popen("bluetoothctl scan on & sleep 5; bluetoothctl devices", "r");
    if (!fp) return;
    char line[128];
    device_count = 0;
    while (fgets(line, sizeof(line), fp) && device_count < MAX_DEVICES) {
        if (strstr(line, "Device")) {
            sscanf(line, "Device %49s", devices[device_count]);
            device_count++;
        }
    }
    pclose(fp);
}

void connect_to_device(int index) {
    if (index < 0 || index >= device_count) return;
    char command[100];
    snprintf(command, sizeof(command), "bluetoothctl connect %s", devices[index]);
    system(command);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Bluetooth Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("main.ttf", 24);
    SDL_Color white = {255, 255, 255, 255};

    scan_bluetooth_devices();

    int selected = 0;
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_DOWN) selected = (selected + 1) % device_count;
                else if (event.key.keysym.sym == SDLK_UP) selected = (selected - 1 + device_count) % device_count;
                else if (event.key.keysym.sym == SDLK_RETURN) connect_to_device(selected);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < device_count; i++) {
            SDL_Surface *surface = TTF_RenderText_Solid(font, devices[i], white);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {50, 50 + i * 30, surface->w, surface->h};
            if (i == selected) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
