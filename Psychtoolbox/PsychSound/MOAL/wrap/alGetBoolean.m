function r = alGetBoolean( param )

% alGetBoolean  Interface to OpenAL function alGetBoolean
%
% usage:  r = alGetBoolean( param )
%
% C function:  ALboolean alGetBoolean(ALenum param)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alGetBoolean', param );

return
