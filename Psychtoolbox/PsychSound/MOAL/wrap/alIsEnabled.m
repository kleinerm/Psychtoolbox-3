function r = alIsEnabled( capability )

% alIsEnabled  Interface to OpenAL function alIsEnabled
%
% usage:  r = alIsEnabled( capability )
%
% C function:  ALboolean alIsEnabled(ALenum capability)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alIsEnabled', capability );

return
