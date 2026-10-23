#ifndef VEHICLE_LOGIC_H
#define VEHICLE_LOGIC_H
#include "common.h"
typedef enum { FRAME_VALID, FRAME_UNKNOWN, FRAME_BAD_PAYLOAD } frame_result_t;
frame_result_t vehicle_validate(const CAN_Frame *frame);
#endif
