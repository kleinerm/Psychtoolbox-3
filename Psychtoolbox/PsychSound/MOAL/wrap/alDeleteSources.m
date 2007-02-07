function alDeleteSources( n, sources )

% alDeleteSources  Interface to OpenAL function alDeleteSources
%
% usage:  alDeleteSources( n, sources )
%
% C function:  void alDeleteSources(ALsizei n, const ALuint* sources)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alDeleteSources', n, uint32(sources) );

return
