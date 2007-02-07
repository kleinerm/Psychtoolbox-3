function alDisable( capability )

% alDisable  Interface to OpenAL function alDisable
%
% usage:  alDisable( capability )
%
% C function:  void alDisable(ALenum capability)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alDisable', capability );

return
