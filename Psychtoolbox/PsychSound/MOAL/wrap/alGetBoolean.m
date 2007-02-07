function r = alGetBoolean( param )

% alGetBoolean  Interface to OpenAL function alGetBoolean
%
% usage:  r = alGetBoolean( param )
%
% C function:  ALboolean alGetBoolean(ALenum param)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alGetBoolean', param );

return
