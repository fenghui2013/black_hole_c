#ifndef _BH_TIMER_H_
#define _BH_TIMER_H_

typedef void * (*bh_timer_handler)(void *);
typedef struct bh_timer bh_timer;

bh_timer * bh_timer_create();
void       bh_timer_release(bh_timer *timer);
void       bh_timer_set(bh_timer *timer, int time, int times, bh_timer_handler handler, void *handler_arg); // time:millisecond
int        bh_timer_get(bh_timer *timer);
void       bh_timer_execute(bh_timer *timer);
#endif
