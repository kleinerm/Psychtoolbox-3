function alEnable( capability )

% alEnable  Interface to OpenAL function alEnable
%
% usage:  alEnable( capability )
%
% C function:  void alEnable(ALenum capability)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alEnable', capability );

return
