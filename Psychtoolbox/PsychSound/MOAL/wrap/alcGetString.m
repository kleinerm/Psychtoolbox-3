function s = alcGetString(enum)

if nargin~=1
    error('Invalid number of parameters.');
end


s=moalcore('alcGetString', int32(enum));

return;
