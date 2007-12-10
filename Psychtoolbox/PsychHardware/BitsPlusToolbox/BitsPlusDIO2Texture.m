function texturePtr = BitsPlusDIO2Texture(windowPtr, mask, data, command)
% texturePtr = BitsPlusClut2Texture(windowPtr, mask, data, command)
%
%   Generates a PsychToolbox texture containing the magic code and data
%   required to set the DIO in Bits++ mode.
%
%   'mask', 'data', and 'command' have the same meaning as in the function
%   'bitsEncodeDIO.m'.

% History:
%
% ??/??/???? Written, probably by Christopher Broussard?
% 12/10/2007 Modified to use common code in BitsPlusDIO2Matrix. (MK)

if nargin ~= 4
    error('Usage: texturePtr = BitsPlusClut2Texture(windowPtr, mask, data, command)');
end

% Call common encoder routine to create T-Lock image matrix:
encodedDIOdata = BitsPlusDIO2Matrix(mask, data, command);

% Generate the texture holding the Bits++ clut.
texturePtr = Screen('MakeTexture', windowPtr, encodedDIOdata);

return;
