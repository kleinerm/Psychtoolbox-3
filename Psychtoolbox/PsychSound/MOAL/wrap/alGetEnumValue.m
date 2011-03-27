function r = alGetEnumValue( ename )

% alGetEnumValue  Interface to OpenAL function alGetEnumValue
%
% usage:  r = alGetEnumValue( ename )
%
% C function:  ALenum alGetEnumValue(const ALchar* ename)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moalcore( 'alGetEnumValue', uint8(ename) );

return
