function s = alGetString(enum)

if nargin~=1
    error('Invalid number of parameters.');
end


s=moalcore('alGetString', int32(enum));

return;
