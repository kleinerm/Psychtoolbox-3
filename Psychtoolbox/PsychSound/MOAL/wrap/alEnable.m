function alEnable( capability )

% alEnable  Interface to OpenAL function alEnable
%
% usage:  alEnable( capability )
%
% C function:  void alEnable(ALenum capability)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alEnable', capability );

return
