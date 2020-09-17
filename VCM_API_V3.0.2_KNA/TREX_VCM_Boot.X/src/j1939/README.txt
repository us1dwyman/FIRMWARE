
J1939 CANBUS Stack

The library has been modified:
* to allow the use of various conditional compiles to limit code space/RAM requirements.  
* To interface to the Control Solutions CAN driver interface

The library expects a header file (j1939cfg.h) to be found in the compiler's include path.  This header file defines the various conditional compiles that control the use of the library.

See an existing j1939cfg.h file for details.

The application is expected to provide a 
void J1939_AppProcess( j1939_t *pMsg );
function, which will be called when incoming messages arrive.

If J1939_ADDRESS_CLAIM is defined, the application must provide a

If J1939_BIP is defined, the application should set the level of babbling idiot protection using the
J1939_bip_tx_rate_allowed_set() function.

If J1939_TP is defined, the library will send and receive messages with more than 8 data bytes.


The library expects that the CAN hardware will be initialized BEFORE the J1939 stack, and that sending and receiving CAN messages is accomplished by calling CAN_Rx() and CAN_Tx().  So the CAN hardware driver must support these features.

For more details, refer to the ssj1939 user's manual.pdf

Files:
* j1939.c : basic j1939 implementation
* j1939tp.c: implements the transport protocol (messages larger than 8 bytes)
* j1939tp_rx.c: implements only the receipt of transport protocol (saves space, if the application only requires receiving large messages)

