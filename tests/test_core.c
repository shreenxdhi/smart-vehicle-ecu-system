#include "protocol_can.h"
#include "vehicle_logic.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    CAN_Frame f={CAN_ID_AIRBAG,1,{90}}, out[8]; int n=8;
    can_reset(42);
    assert(can_send(NULL)==STATUS_ERROR);
    f.id=0x800; assert(can_send(&f)==STATUS_ERROR);
    f.id=CAN_ID_AIRBAG; f.dlc=9; assert(can_send(&f)==STATUS_ERROR);
    f.dlc=1; assert(can_bus_count==0);
    assert(can_receive(out,NULL)==STATUS_ERROR);
    n=-1; assert(can_receive(out,&n)==STATUS_ERROR);
    n=1; assert(can_receive(NULL,&n)==STATUS_ERROR);
    n=0; assert(can_receive(NULL,&n)==STATUS_OK);
    for (unsigned i=0;i<8;++i) { f.id=0x107u-i; assert(can_send(&f)==STATUS_OK); }
    assert(can_send(&f)==STATUS_ERROR && can_bus_count==8);
    n=3; assert(can_receive(out,&n)==STATUS_OK && n==3 && can_bus_count==5);
    for (unsigned i=0;i<3;++i) assert(out[i].id==0x100u+i);
    n=8; assert(can_receive(out,&n)==STATUS_OK && n==5 && can_bus_count==0);
    for (unsigned i=0;i<5;++i) assert(out[i].id==0x103u+i);
    can_reset(1); g_round_robin_fallback=1;
    f.id=CAN_ID_FUEL; assert(can_send(&f)==STATUS_OK);
    for (unsigned cycle=0;cycle<3;++cycle) {
        f.id=CAN_ID_AIRBAG; assert(can_send(&f)==STATUS_OK);
        n=1; assert(can_receive(out,&n)==STATUS_OK);
        if (cycle==1) assert(out[0].id==CAN_ID_FUEL);
    }
    assert(vehicle_validate(NULL)==FRAME_BAD_PAYLOAD);
    for (unsigned id=0;id<4;++id) {
        const unsigned ids[]={CAN_ID_AIRBAG,CAN_ID_OBSTACLE,CAN_ID_FUEL,CAN_ID_ANTITHEFT};
        const unsigned limits[]={100,255,100,1};
        f.id=ids[id]; f.dlc=1;
        for (unsigned v=0;v<256;++v) {
            f.data[0]=(unsigned char)v;
            assert(vehicle_validate(&f)==(v<=limits[id]?FRAME_VALID:FRAME_BAD_PAYLOAD));
        }
        f.dlc=0; assert(vehicle_validate(&f)==FRAME_BAD_PAYLOAD);
        f.dlc=2; assert(vehicle_validate(&f)==FRAME_BAD_PAYLOAD);
    }
    f.id=0x7ff; f.dlc=1; assert(vehicle_validate(&f)==FRAME_UNKNOWN);
    g_can_error_rate=1.0; can_reset(1); assert(can_send(&f)==STATUS_ERROR && can_bus_count==0);
    g_can_error_rate=0.5; int results[32];
    can_reset(123);
    for (unsigned i=0;i<32;++i) { results[i]=can_send(&f); n=8;can_receive(out,&n); }
    can_reset(123);
    for (unsigned i=0;i<32;++i) { assert(can_send(&f)==results[i]); n=8;can_receive(out,&n); }
    puts("PASS core: transport bounds, capacity, priority, partial draining, fairness, 1024 payload values, deterministic faults");
    return 0;
}
