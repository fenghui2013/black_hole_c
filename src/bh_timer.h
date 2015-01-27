#ifndef _BH_TIMER_H_
#define _BH_TIMER_H_

typedef struct bh_timer bh_timer;

bh_timer * bh_timer_create();
void       bh_timer_release(bh_timer *timer);
void       bh_timer_set(bh_timer *timer, int time, int times, char *handler_name); // time:millisecond
int        bh_timer_get(bh_timer *timer);
void       bh_timer_execute(bh_timer *timer);
#endif
