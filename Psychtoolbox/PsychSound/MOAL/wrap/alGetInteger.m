function r = alGetInteger( param )

% alGetInteger  Interface to OpenAL function alGetInteger
%
% usage:  r = alGetInteger( param )
%
% C function:  int alGetInteger(ALenum param)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alGetInteger', param );

return
