#include <stdlib.h>
#include "protocol_can.h"

CAN_Frame can_bus_buffer[MAX_CAN_FRAMES];
int can_bus_count = 0;
double g_can_error_rate = 0.0;
int g_round_robin_fallback = 0;
static int rr_last_processed_id = -1;

int can_send(CAN_Frame *frame)
{
    if (g_can_error_rate > 0.0) {
        double rand_val = (double)rand() / RAND_MAX;
        if (rand_val < g_can_error_rate) {
            printf("[CAN BUS] ERROR: Frame dropped/corrupted! (Collision Simulation)\n");
            return STATUS_ERROR;
        }
    }

    if (can_bus_count >= MAX_CAN_FRAMES) {
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
    int read_count = (*count < can_bus_count) ? *count : can_bus_count;
    if (read_count == 0) {
        *count = 0;
        return STATUS_OK;
    }

    // Sort buffer based on arbitration rules
    for (int i = 0; i < can_bus_count - 1; i++) {
        for (int j = 0; j < can_bus_count - i - 1; j++) {
            int score_j = can_bus_buffer[j].id;
            int score_next = can_bus_buffer[j + 1].id;
            
            // Round-robin fallback shifts the priority space to guarantee fairness
            if (g_round_robin_fallback) {
                if (score_j <= rr_last_processed_id) score_j += 0x1000;
                if (score_next <= rr_last_processed_id) score_next += 0x1000;
            }

            if (score_j > score_next) {
                CAN_Frame tmp  = can_bus_buffer[j];
                can_bus_buffer[j]      = can_bus_buffer[j + 1];
                can_bus_buffer[j + 1]  = tmp;
            }
        }
    }

    // Consume the top 'read_count' frames
    for (int i = 0; i < read_count; i++) {
        frames[i] = can_bus_buffer[i];
    }

    // Update round-robin state
    if (g_round_robin_fallback && read_count > 0) {
        rr_last_processed_id = frames[read_count - 1].id;
    }

    // Shift remaining frames forward
    can_bus_count -= read_count;
    for (int i = 0; i < can_bus_count; i++) {
        can_bus_buffer[i] = can_bus_buffer[i + read_count];
    }

    *count = read_count;
    printf("[CAN RX] Received %d frame(s) (%d left in buffer)\n", read_count, can_bus_count);
    return STATUS_OK;
}

void display_can_frame(const CAN_Frame *frame)
{
    printf("  [Frame] ID: 0x%03X | DLC: %d | Data:", frame->id, frame->dlc);
    for (int i = 0; i < frame->dlc; i++)
        printf(" 0x%02X", frame->data[i]);
    printf("\n");
}
