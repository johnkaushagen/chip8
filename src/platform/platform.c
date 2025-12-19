#include "core/chip8.h"
#include "platform.h"
#include <SDL3/SDL.h>

#include <stdlib.h>

struct platform_t {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int window_scale;
};

struct platform_t* create_platform(const char* title, int emu_w, int emu_h, int window_scale)
{
    struct platform_t* platform = malloc(sizeof(struct platform_t));
    if (!platform) return NULL;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        free(platform);
        return NULL;
    }

    platform->window = SDL_CreateWindow(
        title,
        emu_w * window_scale,
        emu_h * window_scale,
        0
    );

    if (!platform->window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        free(platform);
        return NULL;
    }

    platform->renderer = SDL_CreateRenderer(platform->window, NULL);
    if (!platform->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(platform->window);
        SDL_Quit();
        free(platform);
        return NULL;
    }

    platform->texture = SDL_CreateTexture(
        platform->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        emu_w,
        emu_h
    );
    if (!platform->texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error: %s", SDL_GetError());
        SDL_DestroyRenderer(platform->renderer);
        SDL_DestroyWindow(platform->window);
        SDL_Quit();
        free(platform);
        return NULL;
    }
    return platform;
}

void destroy_platform(struct platform_t* platform)
{
    if (platform) {
        if (platform->texture) SDL_DestroyTexture(platform->texture);
        if (platform->renderer) SDL_DestroyRenderer(platform->renderer);
        if (platform->window) SDL_DestroyWindow(platform->window);
        SDL_Quit();
        free(platform);
    }
}   

bool process_events(struct platform_t* platform)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
    }
    return true;
}

void update_screen(struct platform_t* platform, const uint32_t gfxbuffer[GFX_WIDTH * GFX_HEIGHT]) {
    SDL_UpdateTexture(platform->texture, NULL, gfxbuffer, GFX_WIDTH * sizeof(uint32_t));
    SDL_RenderTexture(platform->renderer, platform->texture, NULL, NULL);
    SDL_RenderPresent(platform->renderer);
}

