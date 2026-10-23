#include "vehicle_logic.h"
frame_result_t vehicle_validate(const CAN_Frame *f)
{
    if (!f || f->dlc != 1u) return FRAME_BAD_PAYLOAD;
    switch (f->id) {
        case CAN_ID_AIRBAG:
        case CAN_ID_FUEL:
            return f->data[0] <= 100u ? FRAME_VALID : FRAME_BAD_PAYLOAD;
        case CAN_ID_ANTITHEFT:
            return f->data[0] <= 1u ? FRAME_VALID : FRAME_BAD_PAYLOAD;
        case CAN_ID_OBSTACLE:
            return FRAME_VALID;
        default:
            return FRAME_UNKNOWN;
    }
}
