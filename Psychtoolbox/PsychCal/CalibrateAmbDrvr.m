function cal = CalibrateAmbDrvr(cal,USERPROMPT,whichMeterType,blankOtherScreen)
% cal =  CalibrateAmbDrvr(cal,USERPROMPT,whichMeterType,blankOtherScreen)
%
% This script does the work for monitor ambient calibration.

% 4/4/94		dhb		Wrote it.
% 8/5/94		dhb, ccc	More flexible interface.
% 9/4/94		dhb		Small changes.
%	10/20/94	dhb		Add bgColor variable.
% 12/9/94   ccc   Nine-bit modification
% 1/23/95		dhb		Pulled out working code to be called from elsewhere.
%						dhb		Make user prompting optional.
% 1/24/95		dhb		Get filename right.
% 12/17/96  dhb, jmk  Remove big bug.  Ambient wasn't getting set.
% 4/12/97   dhb   Update for new toolbox.
% 8/21/97		dhb		Don't save files here.
%									Always measure.
% 4/7/99    dhb   NINEBIT -> NBITS
%           dhb   Handle noMeterAvail, RADIUS switches.
% 9/22/99   dhb, mdr  Make boxRect depend on boxSize, defined up one level.
% 12/2/99   dhb   Put background on after white box for aiming.
% 8/14/00   dhb   Call to CMETER('Frequency') only for OS9.
% 8/20/00   dhb   Remove bits arg to SetColor.
% 8/21/00   dhb   Remove RADIUS arg to MeasMonSpd.
% 9/11/00   dhb   Remove syncMode code, any direct refs to CMETER.
% 9/14/00   dhb   Use OpenWindow to open.
%           dhb   Made it a function.
% 7/9/02    dhb   Get rid of OpenWindow, CloseWindow.
% 9/23/02   dhb, jmh  Force background to zero when measurements come on.
% 2/26/03   dhb   Tidy comments.
% 4/1/03    dhb   Fix ambient averaging.

% Check meter
if ~whichMeterType
	CMCheckInit;
end

% Define device characteristics
bits = cal.describe.dacsize;
nInputLevels = 2^bits;

% User prompt
if USERPROMPT
	if cal.describe.whichScreen == 0
		fprintf('Hit any key to proceed past this message and display a box.\n');
		fprintf('Focus radiometer on the displayed box.\n');
		fprintf('Once meter is set up, hit any key - you will get %g seconds\n',...
                cal.describe.leaveRoomTime);
		fprintf('to leave room.\n');
        FlushEvents;
		GetChar;
	else
		fprintf('Focus radiometer on the displayed box.\n');
		fprintf('Once meter is set up, hit any key - you will get %g seconds\n',...
                cal.describe.leaveRoomTime);
		fprintf('to leave room.\n');
	end
end

% Blank other screen
if blankOtherScreen
    [window1, screenRect1] = Screen('OpenWindow', cal.describe.whichBlankScreen);
	Screen('LoadNormalizedGammaTable', window1, zeros(256, 3));
end

% Blank screen to be measured
[window, screenRect] = Screen('OpenWindow', cal.describe.whichScreen);
if (cal.describe.whichScreen == 0)
	HideCursor;
else
	%Screen('MatlabToFront');
end
Screen('LoadNormalizedGammaTable', window, zeros(256, 3));

% Set CLUT
rClut2 = 0:1:255;
clut2 = [rClut2', rClut2', rClut2'];
Screen('LoadClut', window, clut2);
Screen('Flip', window);

% Draw a box in the center of the screen
boxRect = [0 0 cal.describe.boxSize cal.describe.boxSize];
boxRect = CenterRect(boxRect, screenRect);
Screen('LoadClut', window, [nInputLevels-1, nInputLevels-1, nInputLevels-1], 1, bits);
Screen('FillRect', window, 1, boxRect);
Screen('Flip', window);

% Wait for user
if USERPROMPT == 1
    FlushEvents;
    GetChar;
	fprintf('Pausing for %d seconds ...', cal.describe.leaveRoomTime);
	WaitSecs(cal.describe.leaveRoomTime);
	fprintf(' done\n');
end

% Put in appropriate background.
Screen('LoadClut', window, cal.bgColor', 0, bits);
Screen('Flip', window);

% Start timing
t0 = clock;

ambient = zeros(cal.describe.S(3), 1);
for a = 1:cal.describe.nAverage
    % Measure ambient
    ambient = ambient + MeasMonSpd(window, [0 0 0]', cal.describe.S, 0, whichMeterType);
end
ambient = ambient / cal.describe.nAverage;

% Close the screen
Screen('Close', window);
if cal.describe.whichScreen == 0
	ShowCursor;
end

% Report time
t1 = clock;
fprintf('CalibrateAmbDrvr measurements took %g minutes\n', etime(t1,t0)/60);

% Update structure
Smon = cal.describe.S;
Tmon = WlsToT(Smon);
cal.P_ambient = ambient;
cal.T_ambient = Tmon;
cal.S_ambient = Smon;

% Blank other screen
if blankOtherScreen
	Screen('Close', window1);
end     

