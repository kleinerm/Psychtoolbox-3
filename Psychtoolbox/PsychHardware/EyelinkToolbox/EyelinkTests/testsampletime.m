function testsampletime
% Routine to test timing of functions that sample eye position
% John Palmer, last revised 6/6/01
%
% Most tests are of "getsample" function at the bottom of this program.
% It uses 'newfloatsampleavailable' and 'newestfloatsample'.
%
% To run this test program, do an initial calibration, then press 'O'.
% View several fixation targets turning on and off.  
% Output appears in MATLAB command window.
% As an option, one can disable the calibration and just measure timing.
%
% Part 1:  Measure time between sample events with three time bases:
%   eyelink time stamp in sample (always 4 ms interval)
%   getsecs time at time received (variable 1-6? ms)
%   eyelink time stamp of message sent on receiving sample (also var 1-6? ms)
%   (This last time base requires using EDFVIEW to read sample times by hand.)
% Conclusion:  sampling at roughly 4 ms period but quite variable by getSecs.
% Hint to think about: variability is often identical from trial to trial.
%
% Part 2:  Measure elapsed time of key functions
%    Of particular interest is the time from start recording to when the first
% sample is retrieved.  This value puts an upper bound on the delay between
% recording the eye movement and getting realtime feedback.  In my tests, this
% upper bound is 15-17 ms.  Eyelink suggests the actual delay is about 10 ms.
%
% Part 3:  One can also inspect the EDF file and examine the time stamps.
%    Of particular interest is the time from starting to the first recording.
% I observe a 15 ms delay from startblock to synctime.
% I also observed that the position of the first sample is within 2 ms or less 
% the position of the eye at SYNCTIME.
% In addition, the first sample arrives less than 1 ms after SYNCTIME message.
% This suggests the lag between DOS record and sample at Mac and in 
% transmitting the messages is not more than 1-2 ms.
% There still remains to measure the lag between eye and the DOS record
%
% 5/29/01	Begun based on shorteyelinkdemo and testcalib
% 5/30/01	Added measurements of sample time interval
% 5/31/01	many improvements
% 6/1/01	updated documentation
% 6/4/01	using alt version of initwindow in meyelinkinit2
% 6/5/01	fwc changed to work with new toolbox version 1.1
% 6/6/01	v3, jp:  removed global "el", moved initeyelinkdefine after initialize

fprintf('TestSampleTime, John Palmer, last revised 6/6/01\n');

n = 3;							% try 3 (2,10?) number of repeated analyses
nsamplesdisplayed = 5;			% try 5 (1,10?) number of samples printed
nsamplestotal = 250;			% try 250 (5,1000s) total number of samples
targetsize = 11;				% target diameter in pixels
eye_used = 1;					% 0 for left, 1 for right, 2 for binoc

width=832;						% perfered screen resolution in pixels
height=624;
hz=75;
pixelSize=8;

screen = 0;						% misc variables
filename = 'test.edf';

%set up display
res=NearestResolution(screen, width, height, hz, pixelSize);
[window,screenRect]=SCREEN(screen,'OpenWindow',0,[],res);
white=WhiteIndex(window);
black=BlackIndex(window);
gray=(white+black)/2;
SCREEN(window, 'FillRect',gray);  % clear display
hidecursor;


% initialize system
Eyelink('initialize');			% start eyelink
eyelink('openfile', filename);	% open data file on operater PC

el=initeyelinkdefaults;
el.backgroundcolour = gray;
el.foregroundcolour = white;

dotrackersetup(el);				% call calibration routine (disable for quick test)


% define display target relative to current screen
targetrect = CenterRect( [0 0 targetsize targetsize], screenRect);

% record eye position in loop 
for k = 1:n,							
	fprintf('Measurement block %2.0f -----------------------------------\n\n',k);
	starttime = getsecs;				% measure time for various functions
	dummy = getsecs;					% measure time to measure time
	gettime = getsecs;
	SCREEN(window,'WaitBlanking');		% display target
	blanktime = getsecs;
	SCREEN(window, 'FillOval', el.foregroundcolour, targetrect);
	displaytime = getsecs;
	eyelink('startrecording');
	recordtime = getsecs;
	eyelink('message', 'SYNCTIME');		%zero-plot time for EDFVIEW
	messagetime = getsecs;

	% make multiple samples of eye position
	for i = 1:nsamplestotal+1,
		[t(i),x(i),y(i)] = getsample(eye_used);	% get samples of eye position
		s(i) = getsecs;
		eyelink('message', 'sampletime');	% mark time into edf file
	end

	% stop recording and erase target
	eyelink('stoprecording');
	SCREEN(window,'WaitBlanking');		% erase target
	SCREEN(window, 'FillOval', el.backgroundcolour,targetrect);

	% print out multiple times for inspection
	fprintf('EyeLinkTime, GetSecsTime, dx, dy \n');
	for i = 2:nsamplesdisplayed+1,
		tint(i-1) = t(i)-t(i-1);			% calculate intervals
		sint(i-1) = (s(i)-s(i-1))*1000;		% convert to ms
		fprintf('%5.1f %5.1f %8.1f %8.1f\n',...
				tint(i-1), sint(i-1),x(i)-width/2, y(i)-height/2);	
	end;

	% summary stats of samples
	fprintf('\n');
	fprintf('Statistics for eyelink and getsecs (ms)\n');
	fprintf('Mean time  %5.1f %5.1f\n',mean(tint),mean(sint));
	fprintf('Stdev time %5.1f %5.1f\n',std(tint),std(sint));
	fprintf('Min time   %5.1f %5.1f\n',min(tint),min(sint));
	fprintf('Max time   %5.1f %5.1f\n',max(tint),max(sint));
	fprintf('Count      %5.0f\n',nsamplestotal);

	% print out estimates of times for various functions
	fprintf('\n');
	fprintf('Time estimates for various functions (ms)\n');
	fprintf('getsecs base     %8.3f\n',1000*(gettime - starttime));
	fprintf('waitblanking     %8.3f\n',1000*(blanktime - gettime));
	fprintf('display          %8.3f\n',1000*(displaytime - blanktime));
	fprintf('startrecording   %8.3f\n',1000*(recordtime - displaytime));
	fprintf('sending message  %8.3f\n',1000*(messagetime - recordtime));	
	fprintf('first get sample %8.3f\n',1000*(s(1) - messagetime));
	fprintf('\n');
	
	waitsecs(0.5);
end

% exit
SCREEN(window,'close');
eyelink('closefile');
eyelink('shutdown');

% End of program
%--------------------------------------------------------------------
% Functions used in the Program
% ----------------------------------------------------------
function [t,x,y]= getsample(eye_used);
% getsample returns the time and position of the requested eye

while ~Eyelink('newfloatsampleavailable')==1; end;	% wait till new sample
evt = Eyelink('newestfloatsample'); 	% get the sample
x = evt.gx(eye_used+1);      			% get gaze position from sample 
y = evt.gy(eye_used+1);
t = evt.time;							% get time stamp from sample

%--------------------------------------------------------------------
% End of functions
