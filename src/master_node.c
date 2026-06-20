#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "protocol_can.h"
#include "dtc_logger.h"

CAN_Frame run_airbag_ecu(int impact);
CAN_Frame run_obstacle_ecu(int distance_cm);
CAN_Frame run_fuel_ecu(int fuel_pct);
CAN_Frame run_antitheft_ecu(int tamper);

void uart_send(char *message);
void i2c_send(unsigned char address, unsigned char data);

static void process_frames(CAN_Frame *frames, int count)
{
    int alerts = 0;
    
    // Track which ECUs reported in to detect communication loss
    int seen_airbag = 0;
    int seen_obstacle = 0;
    int seen_antitheft = 0;
    int seen_fuel = 0;

    printf("\n--- Processing %d CAN Frames ---\n", count);

    for (int i = 0; i < count; i++) {
        unsigned char val = frames[i].data[0];

        switch (frames[i].id) {
            case CAN_ID_AIRBAG:
                seen_airbag = 1;
                if (val > IMPACT_THRESHOLD) {
                    printf("  *** ACTION: DEPLOY AIRBAG ***\n");
                    log_dtc(DTC_AIRBAG);
                    alerts++;
                }
                break;

            case CAN_ID_OBSTACLE:
                seen_obstacle = 1;
                if (val < OBSTACLE_THRESHOLD) {
                    printf("  *** ACTION: EMERGENCY BRAKE ***\n");
                    log_dtc(DTC_OBSTACLE);
                    alerts++;
                }
                break;

            case CAN_ID_ANTITHEFT:
                seen_antitheft = 1;
                if (val == TAMPER_DETECTED) {
                    printf("  *** ACTION: ALARM ON ***\n");
                    log_dtc(DTC_ANTITHEFT);
                    alerts++;
                }
                break;

            case CAN_ID_FUEL:
                seen_fuel = 1;
                if (val < FUEL_LOW_THRESHOLD) {
                    printf("  *** ACTION: LOW FUEL WARNING ***\n");
                    log_dtc(DTC_FUEL);
                    alerts++;
                }
                break;

            default:
                printf("[MASTER] Unknown CAN ID 0x%03X\n", frames[i].id);
                log_dtc(DTC_UNKNOWN_CAN);
                break;
        }
    }

    // Degraded Mode Check
    if (!seen_airbag) {
        printf("  *** DEGRADED MODE: Airbag ECU Comm Lost! ***\n");
        log_dtc(DTC_LOST_AIRBAG);
        alerts++;
    }
    if (!seen_obstacle) {
        printf("  *** DEGRADED MODE: Obstacle ECU Comm Lost! ***\n");
        log_dtc(DTC_LOST_OBST);
        alerts++;
    }
    if (!seen_antitheft) {
        printf("  *** DEGRADED MODE: Anti-Theft ECU Comm Lost! ***\n");
        log_dtc(DTC_LOST_ANTI);
        alerts++;
    }
    if (!seen_fuel) {
        printf("  *** DEGRADED MODE: Fuel ECU Comm Lost! ***\n");
        log_dtc(DTC_LOST_FUEL);
        alerts++;
    }

    printf("\n--- SYSTEM STATUS ---\n");
    if (alerts == 0)
        printf("  ALL SYSTEMS NORMAL\n");
    else
        printf("  %d ALERT(S) ACTIVE\n", alerts);
}

int main(int argc, char *argv[])
{
    if (argc < 5) {
        printf("Usage: %s <impact> <distance_cm> <fuel_pct> <tamper> [--error-rate <rate>] [--fault-inject] [--starve-test] [--round-robin]\n", argv[0]);
        return STATUS_ERROR;
    }

    int impact       = atoi(argv[1]);
    int distance_cm  = atoi(argv[2]);
    int fuel_pct     = atoi(argv[3]);
    int tamper       = atoi(argv[4]);
    
    int inject_fault = 0;
    int starve_test = 0;

    // Parse optional flags
    for (int i = 5; i < argc; i++) {
        if (strcmp(argv[i], "--error-rate") == 0 && i + 1 < argc) {
            g_can_error_rate = atof(argv[++i]);
            if (g_can_error_rate > 0.0) {
                srand((unsigned int)time(NULL)); // seed PRNG for bus error simulation
                printf("[TEST] Error rate set to %.1f%%\n", g_can_error_rate * 100);
            }
        } else if (strcmp(argv[i], "--fault-inject") == 0) {
            inject_fault = 1;
        } else if (strcmp(argv[i], "--starve-test") == 0) {
            starve_test = 1;
        } else if (strcmp(argv[i], "--round-robin") == 0) {
            g_round_robin_fallback = 1;
        }
    }

    printf("Starting ECU simulation...\n\n");

    if (starve_test) {
        printf("=== RUNNING PRIORITY STARVATION TEST ===\n");
        if (g_round_robin_fallback) printf("Round-Robin Scheduler: ENABLED\n");
        else printf("Round-Robin Scheduler: DISABLED (Strict Priority)\n");

        for (int cycle = 1; cycle <= 5; cycle++) {
            printf("\n[CYCLE %d]\n", cycle);
            
            // High-priority ECU floods the bus
            run_airbag_ecu(impact);
            run_airbag_ecu(impact);
            
            // Low-priority ECU fires a critical alert
            if (cycle == 1) {
                run_fuel_ecu(fuel_pct);
            }
            
            // Artificial CPU constraint: Master can only process 2 frames per cycle
            CAN_Frame received[MAX_CAN_FRAMES];
            int frame_limit = 2; 
            can_receive(received, &frame_limit);
            process_frames(received, frame_limit);
        }
        return STATUS_OK;
    }

    run_airbag_ecu(impact);
    run_obstacle_ecu(distance_cm);
    run_antitheft_ecu(tamper);
    run_fuel_ecu(fuel_pct);

    if (inject_fault) {
        printf("[TEST] Injecting malformed CAN frame...\n");
        CAN_Frame bad_frame;
        bad_frame.id = 0x7FF; // Out-of-range/unknown ID
        bad_frame.dlc = 15;   // Invalid DLC (> 8)
        memset(bad_frame.data, 0xFF, 8); // Corrupted data byte
        can_send(&bad_frame);
    }

    CAN_Frame received[MAX_CAN_FRAMES];
    int frame_count = MAX_CAN_FRAMES; // allow reading up to MAX_CAN_FRAMES
    can_receive(received, &frame_count);

    process_frames(received, frame_count);

    // UART/I2C demo — shows why CAN is better for multi-ECU networks
    printf("\n--- Protocol Demos ---\n");
    uart_send("All ECUs online");
    i2c_send(0x3C, 0x01);

    return STATUS_OK;
}
