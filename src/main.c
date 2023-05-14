#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define RIOS_TASK_COUNT 4
#define IMPL_TASKS 3

static const uint TASK1_PERIOD = 20;
static const uint TASK2_PERIOD = 40;
static const uint TASK3_PERIOD = 100;

static const double TICK_MS = 400.0;

static const double TIMER_TICK_RATE = 20;

static const uint8_t IDLE_TASK = 255;

typedef struct task
{
  bool running;
  uint8_t state;
  uint32_t period;
  uint32_t elapsed_time;
  uint8_t (*tick_fun) (uint8_t state);
}
task_t;
static task_t tasks[RIOS_TASK_COUNT];

static uint8_t running_tasks[RIOS_TASK_COUNT + 1] = {IDLE_TASK};
static uint8_t current_task = 0;

static uint32_t schedule_time = 0;

static repeating_timer_t timer;

static bool timer_callback(repeating_timer_t *timer)
{
  uint8_t i;

  for (i = 0; i <= IMPL_TASKS; ++i)
  {
    if (
      (tasks[i].elapsed_time >= tasks[i].period) && // the task is ready
      (running_tasks[current_task] > i) &&          // the task priority is greater than the current task priority
      (!tasks[i].running)                           // the task is not already running
    )
    {
      tasks[i].elapsed_time = 0;
      tasks[i].running = true;
      current_task++;
      running_tasks[current_task] = i;

      tasks[i].state = tasks[i].tick_fun(tasks[i].state);

      tasks[i].running = 0;
      running_tasks[current_task] = IDLE_TASK;
      current_task--;
    }

    tasks[i].elapsed_time += TIMER_TICK_RATE;
  }

  return true;
}

uint8_t tick1(uint8_t state)
{
  if (++state % 2) {
    gpio_put(2, true);
  } else {
    gpio_put(2, false);
  }
  sleep_ms(10);

  return state;
}

uint8_t tick2(uint8_t state)
{
  if (++state % 2) {
    gpio_put(3, true);
  } else {
    gpio_put(3, false);
  }
  sleep_ms(10);

  return state;
}

uint8_t tick3(uint8_t state)
{
  if (++state % 2) {
    gpio_put(4, true);
  } else {
    gpio_put(4, false);
  }
  sleep_ms(10);

  return state;
}

void main()
{
  tasks[0].state = -1;
  tasks[0].period = TASK1_PERIOD;
  tasks[0].elapsed_time = tasks[0].period;
  tasks[0].running = false;
  tasks[0].tick_fun = &tick1;

  tasks[1].state = -1;
  tasks[1].period = TASK2_PERIOD;
  tasks[1].elapsed_time = tasks[1].period;
  tasks[1].running = false;
  tasks[1].tick_fun = &tick2;

  tasks[2].state = -1;
  tasks[2].period = TASK3_PERIOD;
  tasks[2].elapsed_time = tasks[2].period;
  tasks[2].running = false;
  tasks[2].tick_fun = &tick3;

  gpio_init(2);
  gpio_init(3);
  gpio_init(4);

  add_repeating_timer_ms(TIMER_TICK_RATE, &timer_callback, NULL, &timer);

  for (;;) tight_loop_contents();
}