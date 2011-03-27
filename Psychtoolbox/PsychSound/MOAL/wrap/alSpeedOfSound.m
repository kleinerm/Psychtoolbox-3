function alSpeedOfSound( value )

% alSpeedOfSound  Interface to OpenAL function alSpeedOfSound
%
% usage:  alSpeedOfSound( value )
%
% C function:  void alSpeedOfSound(ALfloat value)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alSpeedOfSound', value );

return
