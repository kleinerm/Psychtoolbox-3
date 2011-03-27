function alDeleteBuffers( n, buffers )

% alDeleteBuffers  Interface to OpenAL function alDeleteBuffers
%
% usage:  alDeleteBuffers( n, buffers )
%
% C function:  void alDeleteBuffers(ALsizei n, const ALuint* buffers)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alDeleteBuffers', n, uint32(buffers) );

return
