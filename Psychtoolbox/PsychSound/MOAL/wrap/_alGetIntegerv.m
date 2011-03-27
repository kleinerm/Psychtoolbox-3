function data = alGetIntegerv( param )

% alGetIntegerv  Interface to OpenAL function alGetIntegerv
%
% usage:  data = alGetIntegerv( param )
%
% C function:  void alGetIntegerv(ALenum param, ALint* data)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

data = int32(0);

moalcore( 'alGetIntegerv', param, data );

return
