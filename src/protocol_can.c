#include <stdint.h>
#include "protocol_can.h"

CAN_Frame can_bus_buffer[MAX_CAN_FRAMES];
int can_bus_count = 0;
double g_can_error_rate = 0.0;
int g_round_robin_fallback = 0;
static int rr_last_processed_id = -1;

static uint32_t random_state = 1;
void can_reset(unsigned int seed)
{
    can_bus_count = 0;
    rr_last_processed_id = -1;
    random_state = seed ? seed : 1u;
}
static uint32_t next_random(void)
{
    uint32_t x = random_state;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    random_state = x;
    return x;
}
int can_send(const CAN_Frame *frame)
{
    if (!frame || frame->id > 0x7ffu || frame->dlc > 8u) {
        printf("[CAN ERROR] Invalid CAN frame rejected\n");
        return STATUS_ERROR;
    }
    if (g_can_error_rate > 0.0) {
        double rand_val = (double)next_random() / 4294967296.0;
        if (rand_val < g_can_error_rate) {
            printf("[CAN BUS] ERROR: Frame dropped by fault model\n");
            return STATUS_ERROR;
        }
    }

    if (can_bus_count >= MAX_CAN_FRAMES) {
        printf("[CAN ERROR] Bus full. Dropping ID=0x%03X\n", frame->id);
        return STATUS_ERROR;
    }

    can_bus_buffer[can_bus_count++] = *frame;

    printf("[CAN TX] ID: 0x%03X | DLC: %d | Data: %d\n",
           frame->id, frame->dlc, frame->dlc ? frame->data[0] : 0);

    return STATUS_OK;
}

int can_receive(CAN_Frame *frames, int *count)
{
    if (!count || *count < 0 || (*count > 0 && !frames)) return STATUS_ERROR;
    int read_count = (*count < can_bus_count) ? *count : can_bus_count;
    if (read_count == 0) {
        *count = 0;
        return STATUS_OK;
    }

    for (int i = 0; i < can_bus_count - 1; i++) {
        for (int j = 0; j < can_bus_count - i - 1; j++) {
            int score_j = can_bus_buffer[j].id;
            int score_next = can_bus_buffer[j + 1].id;
            
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

    for (int i = 0; i < read_count; i++) {
        frames[i] = can_bus_buffer[i];
    }

    if (g_round_robin_fallback && read_count > 0) {
        rr_last_processed_id = frames[read_count - 1].id;
    }

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
    if (!frame || frame->dlc > 8u || frame->id > 0x7ffu) return;
    printf("  [Frame] ID: 0x%03X | DLC: %d | Data:", frame->id, frame->dlc);
    for (int i = 0; i < frame->dlc; i++)
        printf(" 0x%02X", frame->data[i]);
    printf("\n");
}
