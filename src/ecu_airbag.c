#include "common.h"
#include "protocol_can.h"

CAN_Frame run_airbag_ecu(int impact)
{
    CAN_Frame frame;

    printf("[ECU: Airbag] Impact sensor: %d\n", impact);

    frame.id      = CAN_ID_AIRBAG;
    frame.dlc     = CAN_DLC_1_BYTE;
    frame.data[0] = (unsigned char)impact;
    memset(&frame.data[1], 0, 7);

    if (impact > IMPACT_THRESHOLD)
        printf("[ECU: Airbag] Crash detected!\n");

    can_send(&frame);
    return frame;
}
