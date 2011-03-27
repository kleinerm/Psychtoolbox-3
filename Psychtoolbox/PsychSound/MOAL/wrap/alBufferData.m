function alBufferData( bid, format, data, size, freq )

% alBufferData  Interface to OpenAL function alBufferData
%
% usage:  alBufferData( bid, format, data, size, freq )
%
% C function:  void alBufferData(ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moalcore( 'alBufferData', bid, format, data, size, freq );

return
