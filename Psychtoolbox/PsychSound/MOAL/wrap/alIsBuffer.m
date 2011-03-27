function r = alIsBuffer( bid )

% alIsBuffer  Interface to OpenAL function alIsBuffer
%
% usage:  r = alIsBuffer( bid )
%
% C function:  ALboolean alIsBuffer(ALuint bid)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alIsBuffer', bid );

return
