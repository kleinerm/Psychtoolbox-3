function eltime=geteyelinktime(el, maxwait)

% USAGE: eltime=geteyelinktime(el [, maxwait])
% get tracker time
% el: is eyelinkdefault structure
% maxwait: specifies maximum time to wait for a tracker time event
% if omitted, will wait indefinetely

% history
% 28-10-02	fwc created it, based on 'EXGetEyeLinkTime.m' of jp
% 10-12-02	fwc	no longer quits on errors, but returns -1;
%				return getsecs time in dummymode
% 23-12-02	fwc	fixed small error in setting usemaxwait
% 17-02-03	fwc fixed serious bug due to what very often eltime would be set to 0

eltime=-1;
time=0;

if ~exist('maxwait', 'var') | isempty(maxwait) | maxwait <= 0
	usemaxwait=0; % do not timeout
else
	usemaxwait=1;
end

% get Eyelink time
start=getsecs;
if eyelink('isconnected') == el.connected % readtime doesn't work in dummymode
	if Eyelink('requesttime') ~= 0			% request time over network
		fprintf('geteyelinktime: requesttime error');
		return;
	end;
	while time == 0
		if eyelink('isconnected') == el.notconnected
			fprintf('geteyelinktime: lost connection during readtime request');
			return;
		end
	
		if usemaxwait==1 & getsecs-start>maxwait	% repeat until available or timeout
			fprintf('geteyelinktime: timeout of time request');
			return;
		end
		time = Eyelink('readtime');	
	end
	eltime=time;
elseif eyelink('isconnected') == el.dummyconnected
	eltime=getsecs;
end
