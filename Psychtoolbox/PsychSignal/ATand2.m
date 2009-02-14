function ang = ATand2(y,x)
% ang = ATand2(y,x)
% returns angle from -180 to 180 degrees based on x an y coords
% degree version of atan2

% DN 2008-09-11 wrote it

ang = atan2(y,x)./pi.*180;
