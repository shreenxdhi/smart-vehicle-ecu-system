#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "protocol_can.h"

CAN_Frame run_airbag_ecu(int impact);
CAN_Frame run_obstacle_ecu(int distance_cm);
CAN_Frame run_fuel_ecu(int fuel_pct);
CAN_Frame run_antitheft_ecu(int tamper);

void uart_send(char *message);
void i2c_send(unsigned char address, unsigned char data);

static void process_frames(CAN_Frame *frames, int count)
{
    int alerts = 0;

    printf("\n--- Processing %d CAN Frames ---\n", count);

    for (int i = 0; i < count; i++) {
        unsigned char val = frames[i].data[0];

        switch (frames[i].id) {
            case CAN_ID_AIRBAG:
                if (val > IMPACT_THRESHOLD) {
                    printf("  *** ACTION: DEPLOY AIRBAG ***\n");
                    alerts++;
                }
                break;

            case CAN_ID_OBSTACLE:
                if (val < OBSTACLE_THRESHOLD) {
                    printf("  *** ACTION: EMERGENCY BRAKE ***\n");
                    alerts++;
                }
                break;

            case CAN_ID_ANTITHEFT:
                if (val == TAMPER_DETECTED) {
                    printf("  *** ACTION: ALARM ON ***\n");
                    alerts++;
                }
                break;

            case CAN_ID_FUEL:
                if (val < FUEL_LOW_THRESHOLD) {
                    printf("  *** ACTION: LOW FUEL WARNING ***\n");
                    alerts++;
                }
                break;

            default:
                printf("[MASTER] Unknown CAN ID 0x%03X\n", frames[i].id);
                break;
        }
    }

    printf("\n--- SYSTEM STATUS ---\n");
    if (alerts == 0)
        printf("  ALL SYSTEMS NORMAL\n");
    else
        printf("  %d ALERT(S) ACTIVE\n", alerts);
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Usage: %s <impact> <distance_cm> <fuel_pct> <tamper>\n", argv[0]);
        return STATUS_ERROR;
    }

    int impact      = atoi(argv[1]);
    int distance_cm = atoi(argv[2]);
    int fuel_pct    = atoi(argv[3]);
    int tamper      = atoi(argv[4]);

    printf("Starting ECU simulation...\n\n");

    run_airbag_ecu(impact);
    run_obstacle_ecu(distance_cm);
    run_antitheft_ecu(tamper);
    run_fuel_ecu(fuel_pct);

    CAN_Frame received[MAX_ECU_COUNT];
    int frame_count = 0;
    can_receive(received, &frame_count);

    process_frames(received, frame_count);

    // UART/I2C demo — shows why CAN is better for multi-ECU networks
    printf("\n--- Protocol Demos ---\n");
    uart_send("All ECUs online");
    i2c_send(0x3C, 0x01);

    return STATUS_OK;
}
