function r = alcGetEnumValue( ename )

% alcGetEnumValue  Interface to OpenAL function alcGetEnumValue
%
% usage:  r = alcGetEnumValue( ename )
%
% C function:  ALenum alcGetEnumValue(ALDevice* device, const ALchar* ename)

% 15-Jul-2019 -- created

%---protected---
%---skip---

if nargin~=1
    error('invalid number of arguments');
end

r = moalcore( 'alcGetEnumValue', uint8(ename) );

return
