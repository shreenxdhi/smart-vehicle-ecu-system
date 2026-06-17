#ifndef PROTOCOL_CAN_H
#define PROTOCOL_CAN_H

#include "common.h"

extern CAN_Frame can_bus_buffer[MAX_ECU_COUNT];
extern int can_bus_count;

int  can_send(CAN_Frame *frame);
int  can_receive(CAN_Frame *frames, int *count);
void display_can_frame(const CAN_Frame *frame);

#endif
