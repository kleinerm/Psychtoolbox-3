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

help DaqCodes