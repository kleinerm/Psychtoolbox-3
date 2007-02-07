function alBufferfv( bid, param, values )

% alBufferfv  Interface to OpenAL function alBufferfv
%
% usage:  alBufferfv( bid, param, values )
%
% C function:  void alBufferfv(ALuint bid, ALenum param, const ALfloat* values)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moalcore( 'alBufferfv', bid, param, moglsingle(values) );

return
