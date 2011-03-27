function alDisable( capability )

% alDisable  Interface to OpenAL function alDisable
%
% usage:  alDisable( capability )
%
% C function:  void alDisable(ALenum capability)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alDisable', capability );

return
