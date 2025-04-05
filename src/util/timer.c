#include <util/timer.h>
#include <util/time.h>

#undef timer_t

game_timer_t timer_create(float ticks_per_second) {
    game_timer_t timer = { 0 };
    timer.speed = 1;
    timer.elapsed_delta = 0;
    timer.adjustment = 1;
    timer.ticks_per_second = ticks_per_second;
    timer.last_system_clock = time_millis();
    timer.last_hr_clock = time_nano() / 1000000;

    return timer;
}