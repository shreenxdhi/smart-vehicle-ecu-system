#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>

// Lower CAN ID = higher priority
#define CAN_ID_AIRBAG     0x100
#define CAN_ID_OBSTACLE   0x110
#define CAN_ID_ANTITHEFT  0x120
#define CAN_ID_FUEL       0x130

#define IMPACT_THRESHOLD      80
#define OBSTACLE_THRESHOLD    20
#define FUEL_LOW_THRESHOLD    15
#define TAMPER_DETECTED        1

#define CAN_DLC_1_BYTE   1
#define CAN_DLC_2_BYTES  2

#define STATUS_OK    0
#define STATUS_ERROR 1

typedef struct {
    unsigned int  id;
    unsigned char dlc;
    unsigned char data[8];
} CAN_Frame;

#define MAX_ECU_COUNT 4
#define MAX_CAN_FRAMES 8

#endif
