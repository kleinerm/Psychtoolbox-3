function r = alGetDouble( param )

% alGetDouble  Interface to OpenAL function alGetDouble
%
% usage:  r = alGetDouble( param )
%
% C function:  double alGetDouble(ALenum param)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alGetDouble', param );

return
