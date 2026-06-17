#include "common.h"
#include "protocol_can.h"

CAN_Frame run_antitheft_ecu(int tamper)
{
    CAN_Frame frame;

    printf("[ECU: Anti-Theft] Tamper flag: %d\n", tamper);

    frame.id      = CAN_ID_ANTITHEFT;
    frame.dlc     = CAN_DLC_1_BYTE;
    frame.data[0] = (unsigned char)tamper;
    memset(&frame.data[1], 0, 7);

    if (tamper == TAMPER_DETECTED)
        printf("[ECU: Anti-Theft] Tamper detected!\n");

    can_send(&frame);
    return frame;
}
