function alDopplerFactor( value )

% alDopplerFactor  Interface to OpenAL function alDopplerFactor
%
% usage:  alDopplerFactor( value )
%
% C function:  void alDopplerFactor(ALfloat value)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alDopplerFactor', value );

return
