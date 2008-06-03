function encodedDIOdata = BitsPlusDIO2Matrix(mask, data, command)
% encodedDIOdata = BitsPlusDIO2Matrix(mask, data, command);
%
% Generates a Matlab matrix containing the magic code and data
% required to set the DIO port of CRS Bits++ box in Bits++ mode.
%
% 'mask', 'data', and 'command' have the same meaning as in the function
% 'bitsEncodeDIO.m'.
%
% This is a helper function, called by bitsEncodeDIO and
% BitsPlusDIO2Texture, as well as from BitsPlusPlus when used with the
% imaging pipeline. It takes parameters for controlling Bits++ DIO and
% generates the data matrix for the corresponding T-Lock control code. This
% matrix is then used by the respective calling routines to convert it into
% a texture, a framebuffer image, or whatever is appropriate.
%
% This is just to unify the actual T-Lock encoding process in one file, so
% we don't have to edit or fix multiple files if something changes...
%

% History:
% 12/10/2007 Written, derived from BitsPlusDIO2Texture. (MK)
% 06/02/2008 Fix handling of LSB of 'mask': bitand(mask,255) was missing,
%            which would cause wrong result if mask > 255. (MK)

if nargin ~= 3
    error('Usage: encodedDIOdata = BitsPlusDIO2Matrix(mask, data, command)');
end

% Prepare the data array.
encodedDIOdata = uint8(zeros(1, 508, 3));

% Putting the unlock code for DVI Data Packet
encodedDIOdata(1,1:8,1:3) =  ...
    uint8([69  40  19  119 52  233 41  183;  ...
    33  230 190 84  12  108 201 124;  ...
    56  208 102 207 192 172 80  221])';

% Length of a packet - it could be changed
encodedDIOdata(1,9,3) = uint8(249);	% length of data packet = number + 1

% Command - data packet
encodedDIOdata(1,10,3) = uint8(2);          % this is a command from the digital output group
encodedDIOdata(1,10,2) = uint8(command);    % command code
encodedDIOdata(1,10,1) = uint8(6);          % address

% DIO output mask
encodedDIOdata(1,12,3) = uint8(bitand(mask, 255));    % LSB DIO Mask data - Modified by MK, added bitand()!
encodedDIOdata(1,12,2) = uint8(bitshift(mask, -8));   % MSB DIO Mask data
encodedDIOdata(1,12,1) = uint8(7);                    % address

% vectorised
encodedDIOdata(1,14:2:508,3) = uint8(bitand(data, 255));
encodedDIOdata(1,14:2:508,2) = uint8(bitshift(bitand(data, 768), -8));
encodedDIOdata(1,14:2:508,1) = uint8(8:255);

return;
