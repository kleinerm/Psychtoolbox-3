function buffers = alGenBuffers( n )

% alGenBuffers  Interface to OpenAL function alGenBuffers
%
% usage:  buffers = alGenBuffers( n )
%
% C function:  void alGenBuffers(ALsizei n, ALuint* buffers)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

buffers = uint32(zeros(n,1));

moalcore( 'alGenBuffers', n, buffers );

return
