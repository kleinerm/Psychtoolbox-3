function r = alGetFloat( param )

% alGetFloat  Interface to OpenAL function alGetFloat
%
% usage:  r = alGetFloat( param )
%
% C function:  float alGetFloat(ALenum param)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alGetFloat', param );

return
