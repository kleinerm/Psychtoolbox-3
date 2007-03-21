% encodedDIOdata = bitsEncodeDIO(Mask, Data, Command, windowPtr, [setGammaTable])
%   Use it when you want to write DIO synchronised with screen frame.
%   It will sync timer interrupt with each frame.
%
%   'Mask' is DIO mask that must be an integer.
%
%	'Data' is a 248 element array of integers.
%
%	'Command' is the command code.
%
%	'windowPtr' is the window pointer returned by Screen('OpenWindow').
%
%	'setGammaTable' is an optional value that tells the function to set the
%	video card gamma table to a linear function if set to 1.  If set to 0,
%	the gamma table will be assumed to be linear.  By default, this is set
%	to 1.

% 18/4/05 ejw Converted it to run with OSX version of Psychtoolbox

function encodedDIOdata = bitsEncodeDIO(Mask, Data, Command, windowPtr, setGammaTable)
% Make sure the correct number of arguments are passed.
if nargin < 4 || nargin > 5
    error('Usage: encodedDIOdata = bitsEncodeDIO(Mask, Data, Command, windowPtr, [setGammaTable])');
end

% If 'setGammaTable' isn't specified, go ahead and set it.
if nargin == 4
    setGammaTable = 1;
end

% Find out how big the window is.
[screenWidth, screenHeight] = Screen('WindowSize', windowPtr);

% Check that the screen width is at least 512 pixels wide
if screenWidth < 508
    error('window is not big enough to encode the Bits++ digital output');
end

if setGammaTable
    % THE FOLLOWING STEP IS IMPORTANT.
    % make sure the graphics card LUT is set to a linear ramp
    % (else the encoded data will not be recognised by Bits++).
    % There is a bug in some of the OpenGL drivers such that the
    % gamma is incorrectly mapped from [0, 255/256] instead of [0, 1].
    % If you are having trouble using the DIO, try uncommenting the 2nd line
    % below.
    Screen('LoadNormalizedGammaTable', windowPtr, linspace(0, 255/256, 256)' * ones(1, 3));
    %Screen('LoadNormalizedGammaTable', windowPtr, linspace(0, 1, 256)' * ones(1, 3));
end

% Preparing data array
encodedDIOdata = uint8(zeros(1, 508, 3));

% Putting the unlock code for DVI Data Packet
encodedDIOdata(1,1:8,1:3) =  ...
    uint8([69  40  19  119 52  233 41  183;  ...
    33  230 190 84  12  108 201 124;  ...
    56  208 102 207 192 172 80  221])';

% Length of a packet - it could be changed
encodedDIOdata(1,9,3) = uint8(249); % length of data packet = number + 1

% Command - data packet
encodedDIOdata(1,10,3) = uint8(2);          % this is a command from the digital output group
encodedDIOdata(1,10,2) = uint8(Command);    % command code
encodedDIOdata(1,10,1) = uint8(6);          % address

% DIO output mask
encodedDIOdata(1,12,3) = uint8(Mask);                 % LSB DIO Mask data
encodedDIOdata(1,12,2) = uint8(bitshift(Mask, -8));   % MSB DIO Mask data
encodedDIOdata(1,12,1) = uint8(7);                    % address

% vectorised
encodedDIOdata(1,14:2:508,3) = uint8(bitand(Data, 255));
encodedDIOdata(1,14:2:508,2) = uint8(bitshift(bitand(Data, 768), -8));
encodedDIOdata(1,14:2:508,1) = uint8(8:255);

% Display the Tlock packet on the back buffer starting at the bottom
% left.
encodedLineRect = [0, screenHeight - 1, size(encodedDIOdata, 2), screenHeight];
Screen('PutImage', windowPtr, encodedDIOdata, encodedLineRect);

% Make the make buffer current during the next blanking period
Screen('Flip', windowPtr);
