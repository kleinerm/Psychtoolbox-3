function testbutton
% John Palmer
% test button input of eyelink.  Simple version w/o file i/o
% Times using both eyelink timestamp and local computer clock
% runs till holding down key on keyboard while pressing button
% prints out by keypress:  button, states, eyelink time, getsecs time, overhead, diffs
% 7/1/02	bug w/ eyelink time doesn't work, not even intializing variable????
% 7/12/02	new MEX version w/ modified lastbuttonpress, works fine
% 7/15/02	v6:  used lastbuttonpress to get initial time baseline
% 7/16/02	v7:  used currenttime to get initial time
% 7/18/02	v8:  used the new requesttime and readtime routines
% 7/19/02	v9:  broke out routine EXGetEyeLinkTime, v10:  put in separate file
% 7/25/02	fwc renamed to testbutton
%
% Typical Results:
% getsecs overhead 0.02 ms
% EXGetEyeLink overhead 0.60 ms
% random error in eyelink time ~ +-0.50 ms, good for a ms clock!

fprintf('TestButton:  hold down key on keyboard and press button to exit\n');
if eyelink('initialize') 				% initialize eyelink
	error('TestButton:  failure to initialize eyelink'); % abort if not initialized properly
end;	

% Check for keyboard input, till input test for button presses
while kbCheck == 0						% loop until a key pressed	
	starttime0 = getsecs;				% get local start time 
	starttime1 = getsecs;				% get local start time again, check overhead
	estarttime = EXGetEyeLinkTime;		% get eyelink start time
	starttime2 = getsecs;				% local start time after eyelink, ck overhead

	[button,etime] = Eyelink('lastbuttonpress');	% check for button press
	while button == 0					% loop till button press
		[button,etime] = Eyelink('lastbuttonpress');
	end;		
	endtime = getsecs;					% get time done by local computer clock 

% test buttonstates command.  Try multiple keys down to test		
	result = Eyelink('buttonstates');

% print out results for this keypress	
	mytime = (endtime-starttime1)*1000;			%convert to ms
	overhead1 = (starttime1-starttime0)*1000;	% overhead for getsecs
	overhead2 = (starttime2-starttime1)*1000;	% overhead for eyelink
	myetime = (etime-estarttime);				% this timestamp in ms
	fprintf('button %1d, bstates %1d, etime %7.1f,  ctime %7.1f, getsecs %5.2f, eyelink %5.2f,  diff %5.2f\n', ...
		button,result,myetime,mytime, overhead1, overhead2, (mytime - myetime));
	waitsecs(rand/100.);						% wait a random bit to avoid sync clocks
end;

eyelink('shutdown');					% at keypress make clean exit		
