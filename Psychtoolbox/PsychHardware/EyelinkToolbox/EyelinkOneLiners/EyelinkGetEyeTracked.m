function eyetracked=geteyetracked(el)
% get eye information and send message for EDF file
% returns el.RIGHT_EYE or el.LEFT_EYE +1, for array access

% history
% 28-10-02	fwc	extracted from other program
%

eye_used = Eyelink( 'eyeavailable');  % find which eye
switch eye_used 	% select eye, add annotation		
case el.RIGHT_EYE,
	Eyelink('message', 'EYETRACKED RIGHT');
case el.BINOCULAR,
	eye_used = el.LEFT_EYE;
	Eyelink('message', 'EYETRACKED LEFT');
case el.LEFT_EYE,
	Eyelink('message', 'EYETRACKED LEFT');
end
eyetracked=eye_used+1; % we use eyetracked to access array data
