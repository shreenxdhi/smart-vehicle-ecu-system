#ifndef DTC_LOGGER_H
#define DTC_LOGGER_H

// Standardized Diagnostic Trouble Codes (DTCs) for simulation
#define DTC_AIRBAG      "P0001"
#define DTC_OBSTACLE    "P0002"
#define DTC_ANTITHEFT   "P0003"
#define DTC_FUEL        "P0004"

// Loss of Communication DTCs (Degraded Mode)
#define DTC_LOST_AIRBAG "U0101"
#define DTC_LOST_OBST   "U0102"
#define DTC_LOST_ANTI   "U0103"
#define DTC_LOST_FUEL   "U0104"

#define DTC_UNKNOWN_CAN "U0100"

void log_dtc(const char *dtc_code);

#endif
