function seteyelinkdefaults(screenRect)
% function that sets all standard defaults for eyelink used by Shadlen Lab
% individual values can be overwritten by following calls
% Assumes eyelink is already initialized.
% Pass screenRect; nothing returned
% John Palmer, last revised 6/6/01
%
% 6/6/01	Begun based on testcalib and testcalls

if eyelink('isconnected') ~= 1, 				% make sure it is connected
	error('Error in SetEyelinkDefaults:  Eyelink not connected');
	end;

eyelink('command', 'clear_screen 0');	% initialize screen on operater PC

eyelink('command', 'screen_pixel_coords = %d %d %d %d', ...
	screenRect(1),screenRect(2),screenRect(3)-1,screenRect(4)-1);
	
eyelink('message', 'DISPLAY_COORDS %d %d %d %d', ...
	screenRect(1),screenRect(2),screenRect(3)-1,screenRect(4)-1);

eyelink('command', 'active_eye = RIGHT');		% set eye to record
eyelink('command', 'binocular_enabled = NO');	
eyelink('command', 'head_subsample_rate = 0');	% normal (no anti-reflection)
eyelink('command', 'heuristic_filter = ON');	% ON for filter (normal)	
eyelink('command', 'pupil_size_diameter = NO');	% no for pupil area
eyelink('command', 'simulate_head_camera NO');	% NO to use head camera

eyelink('command', 'calibration_type = HV9');
eyelink('command', 'enable_automatic_calibration = YES');	% YES default
eyelink('command', 'automatic_calibration_pacing = 1000');	% 1000 ms default

eyelink('command', 'saccade_velocity_threshold = 35');
eyelink('command', 'saccade_acceleration_threshold = 9500');
eyelink('command', ...
	'file_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON');
	
eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,BUTTON');
eyelink('command', 'link_sample_data  = LEFT,RIGHT,GAZE,AREA');
