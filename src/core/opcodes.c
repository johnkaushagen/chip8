#include "opcodes.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void op_0x00E0_clear_screen(struct chip8_t* chip8) {
    for (int i = 0; i < GFX_WIDTH * GFX_HEIGHT; i++) {
        chip8->gfxbuffer[i] = 0x000000FF;  // RGBA: opaque black
    }
    chip8->draw_flag = true;
}

void op_0x1NNN_jump(struct chip8_t* chip8, uint16_t address) {
    chip8->pc = address;
}

void op_0x6XNN_set_register(struct chip8_t* chip8, uint8_t register_index, uint8_t value) {
    chip8->V[register_index] = value;
}

void op_0x7XNN_add(struct chip8_t* chip8, uint8_t register_index, uint8_t value) {
    chip8->V[register_index] += value;
}

int op_0x8XY0_load(struct chip8_t* chip8, uint8_t reg_x, uint8_t reg_y)
{
    chip8->V[reg_x] = chip8->V[reg_y];
    return 0;
}

void op_0xANNN_set_index(struct chip8_t* chip8, uint16_t nnn) {
    chip8->I = nnn;
}

void op_0xDXYN_draw(struct chip8_t* chip8, uint8_t x, uint8_t y, uint8_t n) {
    uint32_t on  = 0xFFFFFF00; // RGBA: transparent white. Keep alpha = 0 so XOR works correctly

    chip8->V[0xF] = 0;

    uint8_t start_x = (chip8->V[x]) & (GFX_WIDTH - 1);
    uint8_t start_y = (chip8->V[y]) & (GFX_HEIGHT - 1);
    
    for (uint8_t row = 0; row < n; ++row) {
        uint8_t curr_y = start_y + row;
        if (curr_y >= GFX_HEIGHT) break;
        uint8_t sprite_data = chip8->memory[chip8->I + row];
        for (uint8_t col = 0; col < 8; ++col) {
            uint8_t curr_x = start_x + col;
            if (curr_x >= GFX_WIDTH) break;
            if ((sprite_data & (0x80 >> col)) != 0) {
                uint16_t buf_idx = curr_x + (curr_y * GFX_WIDTH);
                if (chip8->gfxbuffer[buf_idx] == on) chip8->V[0xF] = 1;
                chip8->gfxbuffer[buf_idx] ^= on;
            }

        }
    }
    chip8->draw_flag = true;
}
