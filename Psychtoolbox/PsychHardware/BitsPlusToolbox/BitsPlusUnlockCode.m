function unlockCode = BitsPlusUnlockCode
% unlockCode = BitsPlusUnlockCode
% 
% OBSOLETE: BitsPlusEncodeClutRow is now self-sufficient. BitsPlusUnlockCode is no longer needed.
% Fill up an array with the magic numbers required
% to get the Bits++ to use the first line of the
% frame buffer as the CLUT.


% 10/7/02 dhb Modified from code provided by CRS. 

unlockCode = zeros(1,8,3);
unlockCode(1,1,1) = hex2dec('024');
unlockCode(1,1,2) = hex2dec('06A');
unlockCode(1,1,3) = hex2dec('085');
unlockCode(1,2,1) = hex2dec('03F');
unlockCode(1,2,2) = hex2dec('088');
unlockCode(1,2,3) = hex2dec('0A3');
unlockCode(1,3,1) = hex2dec('008');
unlockCode(1,3,2) = hex2dec('013');
unlockCode(1,3,3) = hex2dec('08A');
unlockCode(1,4,1) = hex2dec('0D3');
unlockCode(1,4,2) = hex2dec('019');
unlockCode(1,4,3) = hex2dec('02E');
unlockCode(1,5,1) = hex2dec('003');
unlockCode(1,5,2) = hex2dec('073');
unlockCode(1,5,3) = hex2dec('0A4');
unlockCode(1,6,1) = hex2dec('070');
unlockCode(1,6,2) = hex2dec('044');
unlockCode(1,6,3) = hex2dec('009');
unlockCode(1,7,1) = hex2dec('038');
unlockCode(1,7,2) = hex2dec('029');
unlockCode(1,7,3) = hex2dec('031');
unlockCode(1,8,1) = hex2dec('022');
unlockCode(1,8,2) = hex2dec('09F');
unlockCode(1,8,3) = hex2dec('0D0');

