function alDopplerFactor( value )

% alDopplerFactor  Interface to OpenAL function alDopplerFactor
%
% usage:  alDopplerFactor( value )
%
% C function:  void alDopplerFactor(ALfloat value)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alDopplerFactor', value );

return
