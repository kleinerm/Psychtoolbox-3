function r = alIsExtensionPresent( extname )

% alIsExtensionPresent  Interface to OpenAL function alIsExtensionPresent
%
% usage:  r = alIsExtensionPresent( extname )
%
% C function:  ALboolean alIsExtensionPresent(const ALchar* extname)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alIsExtensionPresent', uint8(extname) );

return
