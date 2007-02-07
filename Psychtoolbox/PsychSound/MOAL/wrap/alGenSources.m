function sources = alGenSources( n )

% alGenSources  Interface to OpenAL function alGenSources
%
% usage:  sources = alGenSources( n )
%
% C function:  void alGenSources(ALsizei n, ALuint* sources)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

sources = uint32(zeros(n,1));

moalcore( 'alGenSources', n, sources );

return
