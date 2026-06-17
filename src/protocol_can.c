#include "protocol_can.h"

CAN_Frame can_bus_buffer[MAX_ECU_COUNT];
int can_bus_count = 0;

int can_send(CAN_Frame *frame)
{
    if (can_bus_count >= MAX_ECU_COUNT) {
        printf("[CAN ERROR] Bus full. Dropping ID=0x%03X\n", frame->id);
        return STATUS_ERROR;
    }

    can_bus_buffer[can_bus_count++] = *frame;

    printf("[CAN TX] ID: 0x%03X | DLC: %d | Data: %d\n",
           frame->id, frame->dlc, frame->data[0]);

    return STATUS_OK;
}

int can_receive(CAN_Frame *frames, int *count)
{
    *count = can_bus_count;
    for (int i = 0; i < can_bus_count; i++)
        frames[i] = can_bus_buffer[i];

    // sort by ID — lower ID wins arbitration
    for (int i = 0; i < *count - 1; i++) {
        for (int j = 0; j < *count - i - 1; j++) {
            if (frames[j].id > frames[j + 1].id) {
                CAN_Frame tmp  = frames[j];
                frames[j]      = frames[j + 1];
                frames[j + 1]  = tmp;
            }
        }
    }

    printf("[CAN RX] Received %d frame(s)\n", *count);
    return STATUS_OK;
}

void display_can_frame(const CAN_Frame *frame)
{
    printf("  [Frame] ID: 0x%03X | DLC: %d | Data:", frame->id, frame->dlc);
    for (int i = 0; i < frame->dlc; i++)
        printf(" 0x%02X", frame->data[i]);
    printf("\n");
}
