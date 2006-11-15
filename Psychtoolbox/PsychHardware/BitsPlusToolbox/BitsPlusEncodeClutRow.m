function bitsPlusClutRow = BitsPlusEncodeClutRow(theClut)
% bitsPlusClutRow = BitsPlusEncodeClutRow(theClut)
%
% This is for the Cambridge Research Systems BITS++ 14-bit video DAC.
% BitsPlusEncodeClutRow accepts the desired content for the CLUT (color
% look up table) and encodes it so that it may be poked into the first (or
% last) line of the image. The BITS++ will detect the "unlock" code and
% transfer the CLUT data into the 256 entry, 3 channel, 14-bit CLUT.
% web http://www.crsltd.com/

% 10/7/02  dhb  Wrote it.
% 10/xx/02 ew   Fixed indexing bug.
% 11/15/02 dgp  Rewrote, reducing run time from 400 ms to 2 ms. Dropped the second argument.
% 20/4/05  ejw  Rescaled to 16 bit values

if nargin~=1
	error('Usage: bitsPlusClutRow=BitsPlusEncodeClutRow(theClut)');
end

bitsPlusClutRow=zeros(1,524,3); % 0.15 ms
bitsPlusClutRow(1,1:8,:)=[36 63 8 211 3 112 56 34; 106 136 19 25 115 68 41 159; 133 163 138 46 164 9 49 208]'; % unlock code. 0.13 ms;
high=floor(theClut/256); % 0.23 ms
low=theClut-256*high; % 0.13 ms
bitsPlusClutRow(1,12+(1:512),1:3)=reshape([high';low'],3,512)'; % 0.56 ms
