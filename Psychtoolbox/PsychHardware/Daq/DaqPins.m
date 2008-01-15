function DaqPins    
% DaqPins 
% USB-1208FS pins 
% 4-channel differential mode         8-channel single-ended mode
% Pin Signal name  Pin Signal name    Pin Signal name  Pin Signal name
%  1  CH0 in high  21  Port A0         1  CH0 in       21  Port A0
%  2  CH0 in low   22  Port A1         2  CH1 in       22  Port A1
%  3  AGND         23  Port A2         3  AGND         23  Port A2
%  4  CH1 in high  24  Port A3         4  CH2 in       24  Port A3
%  5  CH1 in low   25  Port A4         5  CH3 in       25  Port A4
%  6  AGND         26  Port A5         6  AGND         26  Port A5
%  7  CH2 in high  27  Port A6         7  CH4 in       27  Port A6
%  8  CH2 in low   28  Port A7         8  CH5 in       28  Port A7
%  9  AGND         29  GND             9  AGND         29  GND
% 10  CH3 in high  30  +5V out        10  CH6 in       30  +5V out
% 11  CH3 in low   31  GND            11  CH7 in       31  GND
% 12  AGND         32  Port B0        12  AGND         32  Port B0
% 13  D/A out 0    33  Port B1        13  D/A out 0    33  Port B1
% 14  D/A out 1    34  Port B2        14  D/A out 1    34  Port B2
% 15  AGND         35  Port B3        15  AGND         35  Port B3
% 16  CAL          36  Port B4        16  CAL          36  Port B4
% 17  GND          37  Port B5        17  GND          37  Port B5
% 18  TRIG in      38  Port B6        18  TRIG in      38  Port B6
% 19  SYNC         39  Port B7        19  SYNC         39  Port B7
% 20  CTR          40  GND            20  CTR          40  GND
% 
%USB-1408FS same as USB-1208FS except that pin 16 is listed as 2.5 V 
%
% USB-1608FS pins 
% Pin Signal name  Pin Signal name
%  1  CH0 in       21  DIO0
%  2  AGND         22  GND
%  3  CH1 in       23  DIO1
%  4  AGND         24  GND
%  5  CH2 in       25  DIO2
%  6  AGND         26  GND
%  7  CH3 in       27  DIO3
%  8  AGND         28  GND
%  9  CH4 in       29  DIO4
% 10  AGND         30  GND
% 11  CH5 in       31  DIO5
% 12  AGND         32  GND
% 13  CH6 in       33  DIO6
% 14  AGND         34  GND
% 15  Ch7 in       35  DIO7
% 16  AGND         36  SYNC
% 17  CAL          37  TRIG_IN
% 18  AGND         38  CTR
% 19  AGND         39  PC +5V
% 20  AGND         40  GND
%     
% See also Daq, DaqFunctions, DaqTest.
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.
help DaqPins
