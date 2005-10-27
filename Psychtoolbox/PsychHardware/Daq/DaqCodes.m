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
help DaqCodes