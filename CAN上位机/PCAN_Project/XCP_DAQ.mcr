// PCAN-Explorer Macro File
//  First edited: 2025/1/1 23:05:26
FormatVersion=5.0

//XCP Connect
Send 1 1234h 8 ffh 00h cdh cdh cdh cdh cdh cdh
WaitID 1000 1 5678h
If False
	Stop
StartTrace

//Get DAQ processor infomation
Send 1 1234h 8 dah cdh cdh cdh cdh cdh cdh cdh
Wait 100
//Get DAQ resolution infomation
Send 1 1234h 8 d9h cdh cdh cdh cdh cdh cdh cdh
Wait 100
//Get DAQ event infomation
Send 1 1234h 8 d7h cdh 00h 00h cdh cdh cdh cdh
Wait 100
//XCP upload
Send 1 1234h 8 f5h 05h cdh cdh cdh cdh cdh cdh
Wait 100
//Free DAQ
Send 1 1234h 8 d6h cdh cdh cdh cdh cdh cdh cdh
Wait 100
//Alloc DAQ
Send 1 1234h 8 d5h cdh 01h 00h cdh cdh cdh cdh
Wait 100
//Alloc ODT
Send 1 1234h 8 d4h cdh 00h 00h 10h cdh cdh cdh
Wait 100
//Alloc ODT entry
Send 1 1234h 8 d3h cdh 00h 00h 00h 04h cdh cdh
Wait 100
//Set DAQ Ptr
Send 1 1234h 8 e2h cdh 00h 00h 00h 00h cdh cdh
Wait 100
//Write DAQ
Send 1 1234h 8 e1h ffh 04h 00h 38h 00h 00h 20h
Wait 100
//Set DAQ list mode
Send 1 1234h 8 e0h 10h 00h 00h 00h 00h 01h 00h
Wait 100
//Start or Stop DAQ list
Send 1 1234h 8 deh 02h 00h 00h cdh cdh cdh cdh
Wait 100
//Get DAQ Clock
Send 1 1234h 8 dch cdh cdh cdh cdh cdh cdh cdh
Wait 100
//Start or Stop Synch
Send 1 1234h 8 ddh 01h cdh cdh cdh cdh cdh cdh
Wait 100

StopTrace
