// PCAN-Explorer Macro File
// First edited: 2025/5/2 14:23:35
FormatVersion=5.0

// XCP Connect
Send 1 1234h 8 ffh 00h cdh cdh cdh cdh cdh cdh
// Wait ECU Response
WaitID 1000 1 5678h
If False
    Stop
// Polling LOOP :Read Var at 0x20 00 00 3c
Loop_DOOR_LOCK:
StartTrace
//SHORT_LOAD
Send 1 1234h 8 f4h 04h cdh 00h 3ch 00h 00h 20h
Wait 1000

Goto Loop_DOOR_LOCK

StopTrace
Stop
