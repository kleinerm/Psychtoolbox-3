function r = alGetFloat( param )

% alGetFloat  Interface to OpenAL function alGetFloat
%
% usage:  r = alGetFloat( param )
%
% C function:  float alGetFloat(ALenum param)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alGetFloat', param );

return
