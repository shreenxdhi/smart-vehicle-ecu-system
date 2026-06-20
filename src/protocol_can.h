#ifndef PROTOCOL_CAN_H
#define PROTOCOL_CAN_H

#include "common.h"

extern CAN_Frame can_bus_buffer[MAX_CAN_FRAMES];
extern int can_bus_count;
extern double g_can_error_rate;
extern int g_round_robin_fallback;

int  can_send(CAN_Frame *frame);
int  can_receive(CAN_Frame *frames, int *count);
void display_can_frame(const CAN_Frame *frame);

#endif
