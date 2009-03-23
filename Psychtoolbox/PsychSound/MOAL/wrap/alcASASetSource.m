function rc = alcASASetSource(eproperty, source, data)
% rc = alcAsaSetSource(property, sourceid, data);
%
% Interface to function ALEnum alcAsaSetSource(ALuint property, ALuint sourceid, (ALvoid*) data, ALuint size);

% 23-Mar-2009 -- created (MK)

% ---protected---

if nargin < 3
    error('Insufficient number of parameters.');
end

% All parameters are type ALfloat so far:
data = single(data);
nbytes = 4;

rc = moalcore('alcASASetSource', eproperty, uint32(source(1)), data, nbytes);
return;
