function time = EXGetEyeLinkTime
% time = EXGetEyeLinkTime returns the timestamp from the EyeLink.
% John Palmer
% 7/19/02	created
% 17/10/2002 fwc adapted slightly
if Eyelink('requesttime') ~= 0			% request time over network
	error('EXGetEyeLinkTime:  requesttime error');
end;
	
time=0;
while time == 0							% repeat until available
	time = Eyelink('readtime');	
end;
