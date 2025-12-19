cmake_minimum_required(VERSION 3.15...4.0)
project(Chip8 VERSION 0.0.1
              DESCRIPTION "Chip8 interpreter"
              LANGUAGES C)

find_package(SDL3 REQUIRED)

add_executable(chip8 src/main.c
                     src/core/chip8.h
                     src/core/chip8.c
                     src/core/opcodes.h
                     src/core/opcodes.c
                     src/platform/platform.h
                     src/platform/platform.c)
target_include_directories(chip8 PUBLIC src/)
target_link_libraries(chip8 PRIVATE SDL3::SDL3)

enable_testing()

add_executable(run_unit_tests
    tests/unit/core/test_opcodes_unit.c
    src/core/chip8.c
    src/core/opcodes.c
)
target_include_directories(run_unit_tests PRIVATE src tests/vendor)
add_test(NAME chip8_unit_tests COMMAND run_unit_tests)
set_tests_properties(chip8_unit_tests PROPERTIES LABELS "unit")
