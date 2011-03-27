function data = alGetDoublev( param )

% alGetDoublev  Interface to OpenAL function alGetDoublev
%
% usage:  data = alGetDoublev( param )
%
% C function:  void alGetDoublev(ALenum param, ALdouble* data)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

data = double(0);

moalcore( 'alGetDoublev', param, data );

return
