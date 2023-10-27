function testcalls
% quick test program for eyelinktoolbox.
% John Palmer, last revised 6/02/01
%
% Allows one to quickly test if most functions are operational in the toolbox.
% Particularly useful after making updates to the toolbox.
% Also illustrates simple calls to each function.
% Almost all functions included except for calibration.   
% Calibration calls (e.g. trackersetup) is excluded to allow a quick test.
%
% 2/20/01	begun with name "eyelinktest"
% 3/2/01	fixed initialize, added checkexit
% 3/17/01	added receivefile
% 5/25/01	modified to test eyelinktoolbox rather than my eyelinkinterface
% 5/29/01	renamed testcalls.  Added calls to initializedummy
% 6/1/01	added newfloatsampleavailable, newestfloatsample
% 6/2/01	added eyelink commands to define options, currentmode, newsample, etc
% 6/5/01	fwc changed to work with new toolbox version 1.1

fprintf('\nTestCalls Program, John Palmer, last revised 6/5/01\n\n');
filename = 'test.edf';

% test dummy version of initialization sequence
Eyelink('initializedummy');				
fprintf('tested dummy version first\n\n');
eyelink('shutdown');

% standard initialization sequence
status = eyelink('initialize');
if status,
	error('fatal error in intitialization'); 
	else fprintf('standard initialization successful (returns 0):  %d\n',status);
end;	

% define eyelink default settings and constants
fprintf('Initializing defaults and constants\n');
el=initeyelinkdefaults;
disp(el); % display contents of el structure

fprintf('test if connected? (1-connected):  %d\n', eyelink('isconnected'));
fprintf('get current mode? (-1-off, 0-unknown):  %d\n', eyelink('currentmode'));
eyelink('openfile', filename);

% set eyelink options (alternatively one can use defaults in Operater PC)
eyelink('command', 'clear_screen 0');		% clear operater screen 
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
		
% Put trial number at the bottom of operater display
eyelink('command', 'record_status_message ''TRIAL %d''', 1);

eyelink('message', 'TRIAL ID %d', 1);	
eyelink('startrecording');
waitsecs(0.1);
fprintf('test if recording? (0-yes):  %d\n', eyelink('checkrecording'));
eyelink('message', 'DISPLAY_ON');		%marker for RT in analysis
eyelink('message', 'SYNCTIME');			%zero-plot time for EDFVIEW
fprintf('retrive available eye (0-left; 1-right; 2-binoc):  %d\n', ...
		eyelink('eyeavailable'));
fprintf('get current mode? (-1-off, 0-unknown):  %d\n', eyelink('currentmode'));

% test sampling routines
while ~Eyelink('newfloatsampleavailable')==1; end;	% wait till new sample
evt = Eyelink('newestfloatsample'); 	% get the sample
fprintf('\nThe following is the float sample data structure:\n');
disp(evt);								%write out sample

while ~Eyelink('newsampleavailable')==1; end;	% wait till new sample (rtn 1)
evt2 = Eyelink('newestsample'); 	% get the sample
fprintf('\nThe following is the sample data structure:\n');
disp(evt2);								%write out sample

waitsecs(0.5);
eyelink('stoprecording');

% renamed for checkexit to checkrecordexit for toolbox name change
fprintf('check recording exit (0-OK):  %d\n', eyelink('checkrecordexit'));

% exit sequence
fprintf('check closing file (0-OK):  %d\n', eyelink('closefile'));
status = eyelink('receivefile',filename);
fprintf('status of received file (+ is filesize, - is error):  %d\n', status);
eyelink('shutdown');
fprintf('Program Complete\n');
