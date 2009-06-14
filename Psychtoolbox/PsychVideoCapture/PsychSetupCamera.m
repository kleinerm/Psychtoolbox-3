function PsychSetupCamera(camsettingsfile, rate, fullscreen)
% PsychSetupCamera(camsettingsfile, rate)
% 
% Setup basic camera settings like exposure time, brightness and gain.
%
% The routine loads camera settings from 'camsettingsfile' if it
% exists, otherwise it uses the cameras default settings.
%
% Then it runs a video loop at 'rate' frames per second.
%
% During the loop, the user can change settings by pressing:
%
% 'Cursor left' and 'Cursor right' keys to decrease or increase
% exposure time.
%
% 'Cursor up' and 'Cursor down' to increase or decrease camera gain.
%
% 'b' and 'd' to increase or decrease brightness setting.
%
% 'w' to write the current settings to 'camsettingsfile', overwriting
% the old file, if it already exists.
%
% 'ESC'ape key to finish setup.
%
% If no rate is given, the loop runs at 30 Hz.
% If no filename is given, 'CamSettings.mat' is read/written from
% the current working directory.

% History:
% 06/04/06 Written. (MK)

if nargin < 1
   camsettingsfile = 'CamSettings.mat'
end

if nargin < 2
   rate = 30;
end

if nargin < 3
   fullscreen = 0;
end

if IsOctave
   % Disable Octaves output pager.
   more off;

   % Tell Octave to not check timestamps of all M-Files
   % on each invocation -- Saves *a lot* of overhead.
   oldIgnore = ignore_function_time_stamp('all');
end

% Open fullscreen onscreen window with black background
% and everything else at default settings on display 0.
if fullscreen
   win = Screen('OpenWindow', 0, 0);
else
   win = Screen('OpenWindow', 0, 0, [0 0 640 480]);
end

PsychVideoDelayLoop('Verbosity', 2);

% Open video capture device for display in window 'win'.
% We use the default device at default ROI (640 x 480)
% and default color mode.
grabber = PsychVideoDelayLoop('Open', win);

% Measure device capture rate at a requested 'rate' fps,
% tune-in display for optimal presentation.
PsychVideoDelayLoop('TuneVideoRefresh', rate);

% Set abort keys to Escape and 'w':
PsychVideoDelayLoop('SetAbortKeys', [KbName('Escape') , KbName('w')]);

% Set presentation to full field of view, non-mirrored, upside:
PsychVideoDelayLoop('SetPresentation', 1, 0, 0);

% Define time margin for processing and drift correction:
% 9.2 ms are a good value for 60 Hz on a P4 2.2 Ghz...
%PsychVideoDelayLoop('SetHeadstart', 0.0092);
PsychVideoDelayLoop('SetHeadstart', 0.018);

% Enable full logging.
PsychVideoDelayLoop('SetLogging', 1);

% Disable recording:
PsychVideoDelayLoop('RecordFrames', 0);

% Load current camera settings from file:
if exist(camsettingsfile)
   load(camsettingsfile);
   oldcamsettings = PsychCamSettings('SetAllSettings', grabber, camsettings)
   newcamsettings = PsychCamSettings('GetAllSettings', grabber)
end

finish=0;
savesettings=0;

while ~finish
  % Run video loop at req. settings until keypress or timeout:
  rc = PsychVideoDelayLoop('RunLoop', 0, 1);

  % Blank screen at end:
  Screen('Flip', win);

  % Retrieve logs:
  timestamps = PsychVideoDelayLoop('GetLog');

  % Pressing 'Escape' aborts the whole procedure:
  finish = rc.keycode(KbName('Escape'));

  % Pressing 'w' saves the current cam settings to the file:
  savesettings = rc.keycode(KbName('w'));
  if savesettings
     % Save current camera settings to file:
     camsettings = PsychCamSettings('GetAllSettings', grabber)
     save('-mat', camsettingsfile, 'camsettings');
  end;
end;

% Close delay loop, release grabber:
PsychVideoDelayLoop('Close');

% Close all textures and onscreen windows:
Screen('CloseAll');

if IsOctave
   % Reenable timestamp checking.
   ignore_function_time_stamp(oldIgnore);
end

% Plot final timing stats:
delay=0;
plot(timestamps(3,delay+1:end)*1000,';real_delay;', timestamps(2,delay+1:end)*1000, ';cts_to_vbl_delay;');

% For the stats, we throw away the first 4 frames:
statstart = max(delay+1, 4);

mindelay = min(timestamps(3,statstart:end)*1000);
maxdelay = max(timestamps(3,statstart:end)*1000);
meandelay = mean(timestamps(3,statstart:end)*1000);
stddevdelay = std(timestamps(3,statstart:end)*1000);
fprintf('Mindelay %f ms.\n', mindelay);
fprintf('Maxdelay %f ms.\n', maxdelay);
fprintf('Meandelay %f ms, stddev = %f ms.\n', meandelay, stddevdelay);

% Done!
end
