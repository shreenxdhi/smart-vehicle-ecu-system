#include "common.h"
#include "protocol_can.h"

CAN_Frame run_fuel_ecu(int fuel_pct)
{
    CAN_Frame frame;

    printf("[ECU: Fuel] Tank level: %d%%\n", fuel_pct);

    frame.id      = CAN_ID_FUEL;
    frame.dlc     = CAN_DLC_1_BYTE;
    frame.data[0] = (unsigned char)fuel_pct;
    memset(&frame.data[1], 0, 7);

    if (fuel_pct < FUEL_LOW_THRESHOLD)
        printf("[ECU: Fuel] Fuel level critical!\n");

    can_send(&frame);
    return frame;
}
