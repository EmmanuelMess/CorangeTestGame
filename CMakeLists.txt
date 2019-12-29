cmake_minimum_required(VERSION 3.15)
project(TestGame C)

set(CMAKE_C_STANDARD 99)
set(CORANGE_DIR "Corange-master")
set(LIBSPNG_DIR "libspng-0.5.0/build")

include_directories(${CORANGE_DIR}/include/)
include_directories(${LIBSPNG_DIR}/)
include_directories(include)

link_directories(${CORANGE_DIR})
link_directories(${LIBSPNG_DIR})

add_executable(TestGame
    src/character.c src/coin.c src/level.c src/platformer.c src/support/sprite_animation.c include/support/sprite_animation.h src/support/png_support.c include/support/png_support.h src/support/sprite_sheet_support.c include/support/sprite_sheet_support.h src/support/spng_helper.c include/support/spng_helper.h src/animation_manager.c include/animation_manager.h)
target_link_libraries(TestGame spng)
target_link_libraries(TestGame corange)
target_link_libraries(TestGame GL)
target_link_libraries(TestGame SDL2)
target_link_libraries(TestGame SDL2_net)
target_link_libraries(TestGame SDL2_mixer)
target_link_libraries(TestGame m)