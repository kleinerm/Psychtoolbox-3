function testcalib
% simple function to test calibration procedures
% adopted from shorteyelinkdemo
% uses our perfered resolution
% John Palmer, last revised 6/6/01
%
% 5/29/01	Begun
% 6/4/01	using meyelinkinit2 due to revised arg in initwindow
% 6/6/01	updated for eyelinktoolbox 1.1, now uses initeyelinkdefaults

% define various constants
width=832;							% perfered screen resolution in pixels
height=624;
hz=75;
pixelSize=8;
screen = 0;							% misc variables
filename = 'test.edf';	

%set up display
res=NearestResolution(screen, width, height, hz, pixelSize);
[window,screenRect]=SCREEN(screen,'OpenWindow',0,[],res);
white=WhiteIndex(window);
black=BlackIndex(window);
gray=(white+black)/2;
SCREEN(window, 'FillRect',gray); 	 % clear display
hidecursor;

%start program
Eyelink('initialize');
eyelink('openfile', filename);		% open data file on operater PC
el=initeyelinkdefaults;				% define eyelink constants
el.backgroundcolour = gray;
el.foregroundcolour = black;
SCREEN(window, 'FillRect',el.backgroundcolour);  % clear display

% send commands to eyelink to specify modes
eyelink('command', 'screen_pixel_coords = %d %d %d %d', ...
	screenRect(1),screenRect(2),screenRect(3)-1,screenRect(4)-1);
	
eyelink('command', 'calibration_type = HV9');

eyelink('message', 'DISPLAY_COORDS %d %d %d %d', ...
	screenRect(1),screenRect(2),screenRect(3)-1,screenRect(4)-1);
	
eyelink('command', 'saccade_velocity_threshold = 35');

eyelink('command', 'saccade_acceleration_threshold = 9500');

eyelink('command', ...
	'file_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON');
	
eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,BUTTON');

eyelink('command', 'link_sample_data  = LEFT,RIGHT,GAZE,AREA');

% eyelink('trackersetup');			% alternative C version (not used)

dotrackersetup(el);					% MAIN CALL:  this does the calibration

eyelink('startrecording');
eyelink('message', 'DISPLAY_ON');	% message for RT recording in analysis
eyelink('message', 'SYNCTIME');		% zero-plot time for EDFVIEW
waitsecs(1.0);
eyelink('stoprecording');

SCREEN(window,'close');				% exit program
eyelink('closefile');
eyelink('shutdown');
