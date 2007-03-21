function texturePtr = BitsPlusDIO2Texture(windowPtr, mask, data, command)
% texturePtr = BitsPlusClut2Texture(windowPtr, mask, data, command)
%
%   Generates a PsychToolbox texture containing the magic code and data
%   required to set the DIO in Bits++ mode.
%
%   'mask', 'data', and 'command' have the same meaning as in the function
%   'bitsEncodeDIO.m'.

if nargin ~= 4
    error('Usage: texturePtr = BitsPlusClut2Texture(windowPtr, mask, data, command)');
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
encodedDIOdata(1,12,3) = uint8(mask);                 % LSB DIO Mask data
encodedDIOdata(1,12,2) = uint8(bitshift(mask, -8));   % MSB DIO Mask data
encodedDIOdata(1,12,1) = uint8(7);                    % address

% vectorised
encodedDIOdata(1,14:2:508,3) = uint8(bitand(data, 255));
encodedDIOdata(1,14:2:508,2) = uint8(bitshift(bitand(data, 768), -8));
encodedDIOdata(1,14:2:508,1) = uint8(8:255);

% Generate the texture holding the Bits++ clut.
texturePtr = Screen('MakeTexture', windowPtr, encodedDIOdata);
