#include "common.h"
#include "protocol_can.h"

CAN_Frame run_obstacle_ecu(int distance_cm)
{
    CAN_Frame frame;

    printf("[ECU: Obstacle] Distance sensor: %d cm\n", distance_cm);

    frame.id      = CAN_ID_OBSTACLE;
    frame.dlc     = CAN_DLC_1_BYTE;
    frame.data[0] = (unsigned char)distance_cm;
    memset(&frame.data[1], 0, 7);

    if (distance_cm < OBSTACLE_THRESHOLD)
        printf("[ECU: Obstacle] Obstacle too close!\n");

    can_send(&frame);
    return frame;
}
