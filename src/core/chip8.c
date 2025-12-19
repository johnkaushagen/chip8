#include "core/chip8.h"
#include "core/opcodes.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_initialize(struct chip8_t* chip8) {
    const uint16_t font_offset = 0x50;
    memset(chip8, 0, sizeof(*chip8));
    chip8->pc = PC_START;
    memcpy(chip8->memory + font_offset, &chip8_fontset, sizeof(chip8_fontset));
    for (int i = 0; i < GFX_WIDTH * GFX_HEIGHT; i++) {
        chip8->gfxbuffer[i] = 0x000000FF;  // RGBA: opaque black
    }
}

bool chip8_load_rom(struct chip8_t* chip8, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (!rom) {
        fprintf(stderr, "Failed to open ROM: %s\n", filename);
        return false;
    }

    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    if (rom_size > (MEMORY_SIZE - PC_START)) {
        fprintf(stderr, "Error: ROM file too large!\n");
        fclose(rom);
        return false;
    }

    fread(chip8->memory + PC_START, 1, rom_size, rom);
    fclose(rom);
    return true;
}

void chip8_emulate_cycle(struct chip8_t* chip8) {
    // Fetch opcode
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    chip8->pc += 2;

    printf("Read opcode 0x%04X\n", opcode);

    const uint16_t nnn = (opcode & 0x0FFF);
    const uint8_t  nn  = (opcode & 0x00FF);
    const uint8_t  n   = (opcode & 0x000F);
    const uint8_t  x   = (opcode & 0x0F00) >> 8;
    const uint8_t  y   = (opcode & 0x00F0) >> 4;

    // Decode and execute opcode
    switch (opcode & 0xF000) {
        // Opcodes go here
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0: // Clear the display
                    op_0x00E0_clear_screen(chip8);
                    printf("Found opcode 0x00E0: Clear Screen\n");
                    break;
                default:
                    fprintf(stderr, "Unknown opcode: 0x%04X\n", opcode);
                    break;
            }
            break;
        case 0x1000: // Jump to address
            op_0x1NNN_jump(chip8, nnn);
            printf("Found opcode 0x1NNN: Jump with address 0x%04X\n", nnn);
            break;
        case 0x6000: // Set register
            op_0x6XNN_set_register(chip8, x, nn);
            printf("Found opcode 0x6XNN: Set Register\n");
            break;
        case 0x7000: // Add to register
            op_0x7XNN_add(chip8, x, nn);
            printf("Found opcode 0x%04X: Add 0x%02X to register 0x%X.\n", opcode, nn, x);
            break;
        case 0xA000:
            op_0xANNN_set_index(chip8, nnn);
            printf("Found opcode 0x%04X: Setting I to 0x%03X\n", opcode, nnn);
            break;
        case 0xD000: // 0xDXYN draw
            op_0xDXYN_draw(chip8, x, y, n);
            break;
        default:
            fprintf(stderr, "Unknown opcode: 0x%04X\n", opcode);
            break;
    }
    
    

    // Update timers
    // TODO
}
