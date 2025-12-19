#include "core/chip8.h"
#include "platform/platform.h"
#include <SDL3/SDL.h>
#include <stdio.h>

int main()
{
    struct chip8_t chip8;
    chip8_initialize(&chip8);
    const char* rom_path = "roms/ibm.ch8";
    if (!chip8_load_rom(&chip8, rom_path)) {
        return 1;
    }

    struct platform_t* platform = create_platform("CHIP-8 Emulator", GFX_WIDTH, GFX_HEIGHT, 10);
    if (!platform) {
        printf("Failed to create platform\n");
        return 1;
    }

    bool running = true;
    while (running) {
        running = process_events(platform);
        chip8_emulate_cycle(&chip8);
        if (chip8.draw_flag) {
            update_screen(platform, chip8.gfxbuffer);
            chip8.draw_flag = false;
        }
        SDL_Delay(100);
    }

    destroy_platform(platform);
    return 0;
}
