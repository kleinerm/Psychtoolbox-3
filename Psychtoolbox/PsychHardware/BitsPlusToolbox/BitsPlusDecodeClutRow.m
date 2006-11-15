function theClut = BitsPlusDecodeClutRow(bitsPlusClutRow)
% Recover the 256x3 CLUT table from the 1x524x3 encoded form that we write into
% the first line of the Bits++ image.
% NOTE: this routine takes about 100 times longer than necessary, but it's not time-critical.

% 11/5/02  jmh  Wrote it.
% 11/15/02 dgp  Cosmetic editing.

% The bits++ CLUT data is located in columns 13:524, i.e. 12+(1:512). There is a pair of one-byte numbers
% for each of the 14 bit values. The first of each pair holds the most significant part of
% the pixel value and the second holds the least significant part.
% We add up these two parts to get the original CLUT back.

theClut=zeros(256,3);
for i = 1:256
	theClut(i,1:3) = bitsPlusClutRow(1,12+(i-1)*2+1,1:3)*64 + bitsPlusClutRow(1,12+i*2,1:3)/4;
end
