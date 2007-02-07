function data = alGetDoublev( param )

% alGetDoublev  Interface to OpenAL function alGetDoublev
%
% usage:  data = alGetDoublev( param )
%
% C function:  void alGetDoublev(ALenum param, ALdouble* data)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

data = double(0);

moalcore( 'alGetDoublev', param, data );

return
