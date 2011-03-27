function r = alIsExtensionPresent( extname )

% alIsExtensionPresent  Interface to OpenAL function alIsExtensionPresent
%
% usage:  r = alIsExtensionPresent( extname )
%
% C function:  ALboolean alIsExtensionPresent(const ALchar* extname)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alIsExtensionPresent', uint8(extname) );

return
