function alDopplerVelocity( value )

% alDopplerVelocity  Interface to OpenAL function alDopplerVelocity
%
% usage:  alDopplerVelocity( value )
%
% C function:  void alDopplerVelocity(ALfloat value)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alDopplerVelocity', value );

return
