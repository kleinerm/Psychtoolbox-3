function DaqCodes                                       
% Command codes for the USB-1208FS Daq.                                       
% 0x10    16  DaqAIn                  Read analog in          
% 0x11    17  DaqAInScan              Read analog in, clocked                
% 0x12    18  DaqAInStop              Stop input scan                 
% 0x13    19  DaqALoadQueue           Set channel gains                   
% 0x14    20  DaqAOut                 Write analog out            
% 0x15    21  DaqAOutScan             Write analog out, clocked              
% 0x16    22  DaqAOutStop             Stop output scan                
% 0x01     1  DaqDConfigPort          Configure digital port                  
% 0x03     3  DaqDIn                  Read digital ports          
% 0x04     4  DaqDOut                 Write digital port          
% 0x40    64  DaqBlinkLED             Cause LED to blink              
% 0x20    32  DaqCInit                Initialize counter              
% 0x21    33  DaqCIn                  Read counter            
% 0x46    70  DaqGetAll               Read all analog and digital input values                
% 0x44    68  DaqGetStatus            Read device status                  
% 0x41    65  DaqReset                Reset the device                
% 0x45    69  DaqSetCal               Set CAL output              
% 0x43    67  DaqSetSync              Configure sync              
% 0x42    66  DaqSetTrigger           Configure ext. trigger                  
% 0x30    48  DaqMemRead              Read memory                 
% 0x31    49  DaqMemWrite             Write memory                
% 0x55    85  DaqReadCode             Read program memory                 
% 0x50    80  DaqPrepareDownload      Prepare for program memory download                     
% 0x51    81  DaqWriteCode            Write program memory                    
% 0x53    83  DaqWriteSerialNumber    Write a new serial number to device                         
% See also Daq, DaqFunctions, DaqPins.
%
%
%
% Command codes for USB-1608FS.  Since all overlapped codes are the same,
% assume that all codes are the same...  well... except that there are no
% analog output ports apparently, so DaqAOut, DaqAOutScan, and DaqAOutStop
% should not have associated codes.  No 20, 21, or 22.
% 0x01    1   DaqDConfigPort        Configure digital port
% 0x02    2   DaqDConfigPortBit     Configure individual digital port bits
% 0x03    3   DaqDIn                Read digital port
% 0x04    4   DaqDOut               Write digital port
% 0x05    5   DaqDReadBit           Read digital port bit
% 0x06    6   DaqDWriteBit          Write digital port bit
% 
% 0x10   16   DaqAIn                Read analog input channel
% 0x11   17   DaqAInScan            Scan analog channels
% 0x12	 18   DaqAInStop            Stop input scan
% 0x13	 19   DaqALoadQueue         Load the channel/gain queue
% 
% 0x20	 32   DaqCInit              Initialize counter
% 0x21	 33   DaqCIn                Read Counter
% 
% 0x30	 48   DaqMemRead            Read Memory
% 0x31	 49   DaqMemWrite           Write Memory
% 
% 0x40	 64   DaqBlinkLED           Causes LED to blink
% 0x41	 65   DaqReset              Reset USB interface
% 0x42	 66   DaqSetTrigger         Configure external trigger
% 0x43	 67   DaqSetSync            Configure sync input/output
% 0x44	 68   DaqGetStatus          Get device status
% 0x45	 69   DaqSetCal             Set calibration output
% 0x46	 70   DaqGetAll             Get all analog and digital input values
% 
% 0x50	 80   DaqPrepareDownload    Prepare for program memory download
% 0x51	 81   DaqWriteCode          Write program memory
% 0x52	 82   (unwritten)           Return program memory checksum
% 0x53	 83   DaqWriteSerialNumber  Write a new serial number to device
% 0x54	 84   (unwritten)           Update program memory
% 0x55	 85   DaqReadCode           Read program memory
%
%
%
% Command codes for USB-1024LS and similar: Quite different from the 1x08
% series: A speciality is that reportId must be always zero (0) for these
% devices - they use interrupt endpoint 0 transfers. For the same reason,
% output reports must always by 8 bytes in length -- pad them if they're
% shorter! Handling of port C of the device may be a bit quirky, as it is
% treated as two ports, a low port and a high port.
%
% This is based on code and command codes found in usb-1024LS.h and .c,
% subroutine files of the free libhid, a GPL'ed cross platform HID device
% library written by Warren Jasper (<wjasper@tx.ncsu.edu>). It has not been
% tested by me on any actual device! (MK).
%
% 0x0d   13   DaqDConfigPort        Configure digital port (portid == portA = 0x1, portB = 0x4, portClow = 0x8 portCHi = 0x2)
% 0x03    3   DaqDIn                Read digital port
% 0x01    1   DaqDOut               Write digital port
% 0x02    2   DaqDReadBit           Read digital port bit    (unwritten)
% 0x03    3   DaqDWriteBit          Write digital port bit   (unwritten)
% 0x0b	 11   DaqBlinkLED           Causes LED to blink
% 0x11	 17   DaqReset              Reset USB interface      (unwritten)
% 0x05	  5   DaqCInit              Initialize counter
% 0x04	  4   DaqCIn                Read Counter
% 0x0a	 10   DaqWriteCode          Write program memory     (unwritten)
% 0x09	  9   DaqReadCode           Read program memory      (unwritten)
% 0x0c	 12   DaqWriteSerialNumber  Write a new serial number to device (unwritten)

help DaqCodes
