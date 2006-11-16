% CalibrateManualDrvr
%
% Get some manual measurements from radiometer/photometer.

% 3/8/02  dhb, ly  Wrote it.
% 2/21/03 dhb, ly  Specify input units.

if USERPROMPT
	if cal.describe.whichScreen == 0
		fprintf('Hit any key to proceed past this message and display a box.\n');
		fprintf('Insert manual photometer/radiometer.\n');
		fprintf('Once meter is set up, hit any key to proceed\n');
		GetChar;
	else
		fprintf('Insert manual photometer/radiometer.\n');
		fprintf('Once meter is set up, hit any key to proceed\n');
	end
end

% Blank other screen
if blankOtherScreen
    [window, screenRect] = Screen('OpenWindow', cal.describe.whichBlankScreen);
    Screen('LoadNormalizedGammaTable', window, zeros(256, 3));
end

% Blank screen to be measured
[window,screenRect] = Screen('OpenWindow', cal.describe.whichScreen);
if (cal.describe.whichScreen == 0)
	HideCursor;
else
	%Screen('MatlabToFront');
end
Screen('LoadNormalizedGammaTable', window, zeros(256, 3));

% Draw a box in the center of the screen
boxRect = [0 0 cal.describe.boxSize cal.describe.boxSize];
boxRect = CenterRect(boxRect,screenRect);
Screen('LoadClut', window, [nInputLevels-1 nInputLevels-1 nInputLevels-1], 1);
Screen('FillRect', window, 1, boxRect);
Screen('Flip', window);

% Wait for user
if USERPROMPT == 1
  GetChar;
end

% Put correct surround for measurements.
Screen('LoadClut', window, cal.bgColor', 0);

% Put up white
Screen('LoadClut', window, [nInputLevels-1, nInputLevels-1, nInputLevels-1], 1);
Screen('Flip', window);
cal.manual.white = [];
while isempty(cal.manual.white)
	if cal.manual.photometer
		cal.manual.white = input('Enter photometer reading (cd/m2): ');
	else
		cal.manual.white = 1e-6*input('Enter radiometer reading (micro Watts): ');
	end
end

Screen('LoadClut', window, [0, 0, 0], 1);
Screen('Flip', window);
cal.manual.black = [];
while isempty(cal.manual.black)
	if cal.manual.photometer
		cal.manual.black = input('Enter photometer reading (cd/m2): ');
	else
		cal.manual.black = 1e-6*input('Enter radiometer reading (micro Watts): ');
	end
end
cal.manual.increment = cal.manual.white - cal.manual.black;

% Close window
Screen('Close', window);
