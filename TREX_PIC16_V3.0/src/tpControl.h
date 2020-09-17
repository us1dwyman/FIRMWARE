#ifndef TP_CONTROL_H
#define TP_CONTROL_H

typedef enum {
    TP_INIT,
    TP_WAIT_RTS, 
    TP_DATA_COLLECT_START,
    TP_DATA_COLLECT,
    TP_ABORT,
    TP_SEND_ABORT,
    TP_SEND_COMPLETE
} TransportState_e;

extern TransportState_e transportState;

void transport_init(void);
void transport_Machine(void);
uint8_t transport_isActive(void);
#endif

