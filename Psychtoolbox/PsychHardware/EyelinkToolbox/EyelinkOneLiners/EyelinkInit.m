function [result dummy]=EyelinkInit(dummy)

% USAGE: [result dummy]=EyelinkInit([dummy])
% dummy: omit, or set to 0 to attempt initialization,
% set to 1 to force to use initializedummy
% if regular initialization fails, it will provide option
% for dummy initilization
% returns result=1 when succesful, 0 otherwise
% returns 

% history
% 28-10-02	fwc	extracted it from other program
% 09-12-02	fwc	updated to use EyelinkDummyModeDlg instead of Screen('dialog')
%			a little more testing for erroneous initialization
% 280606    fwc updated for OSX version
% 210207    fwc now also returns dummy parameter. Latter has
%           possibly changed

result=0;

if ~exist('dummy', 'var') | isempty(dummy)
	dummy=0;
end

if dummy==1
	if Eyelink('InitializeDummy') ~=0
		return;
	end
else
	if Eyelink('Initialize') ~= 0 % we can't do a regular initialization, so we ask for dummy mode
		if 1==EyelinkDummyModeDlg
			if Eyelink('InitializeDummy') ~=0
				return;
            end
            dummy=1; % changed
		else
			return;
		end
	end
end
result=1;
