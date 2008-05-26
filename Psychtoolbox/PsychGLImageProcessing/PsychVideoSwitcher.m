function varargout = PsychVideoSwitcher(cmd, varargin)
% PsychVideoSwitcher(command [,arg1, arg2, .....]);
%
% Psychtoolbox support for the Xiangru Li et al. "VideoSwitcher" video
% attenuator device for high precision luminance output with up to 16 bits
% luminance resolution.
%
% This routine incorporates code contributed by Xiangru Li for switching
% between monochrome and color display mode and for performing the
% reference Matlab routines for image formatting.
%
% Options: 'command' is a command string, specifying a subcommand. The
% following subcommands are supported with the following options:
%
% Public functions:
%
%
% PsychVideoSwitcher('SwitchMode', win, enableLuminanceMode [, VideoSwitcherIsABox])
% - Switch programmatically between high precision luminance mode and
% standard RGB true color display mode. 'win' is the window handle of the
% associated onscreen window, or a screen handle for the screen to switch.
%
% 'enableLuminanceMode' must be set to 0 to switch to RGB mode, and to 1 to
% switch to high precision luminance mode.
%
% 'VideoSwitcherIsABox' is an optional argument: If set to 1, then perform
% switching procedure for an external (box) device. If set to zero, then
% perform procedure for an internal (PCI card) device. If argument is
% omitted, the proper default type is read from a configuration file.
%
%
% PsychVideoSwitcher('SetTrigger', win, triggermode [,options]);
% - Set trigger mode and options for VideoSwitcher connected to the display
% of onscreen window 'win'.
%
%
%
%
% Internal helper functions for Psychtoolbox - Must not be called from
% normal user code!!
%
% [btrr, lut] = PsychVideoSwitcher('GetDefaultConfig', win);
% - Get default 'btrr' parameter and 'lut' lookup table from configuration
% file and return them.
%
%
% luttexid = PsychVideoSwitcher('GetLUTTexture', lut);
% - Convert blue-to-luminance calibration lookup table 'lut' into a lookup
% table texture for the imaging pipeline, set it up and return a texture
% handle 'luttexid' to it.
%
%
% PsychVideoSwitcher(win);
% - If 'win' is a numeric onscreen window handle, perform all operations to
% implement the green channel trigger functionality for onscreen window
% 'win'. This routine uses MOGL glXXX() functions to implement drawing of
% proper trigger pixel values to the green channel for trigger creation.
%

% History:
% 05/25/08 mk  Initial incomplete implementation. Incorporates code from
%              Xiangru Li in helper subroutines.
%

% GL access is needed for setup of green trigger channel in callback:
global GL;

% Subfunction dispatch:
if nargin < 1
    error('You must provide a command string!');
end

if isscalar(cmd) & isnumeric(cmd)
    % Special callback from within PTB imaging pipeline:
    
    % TODO: Trigger - Runtime routine...
    
    return;
end

if ~ischar(cmd)
    error('Subcommand must be a string!');
end

if strcmpi(cmd, 'MapLuminanceToRGB')
    % Call lum2rgb() function to use simple conversion of luminance image
    % to RGB image:
    varargout{1} = lum2rgb(varargin{:});
    
    return;
end

if strcmpi(cmd, 'MapLuminanceToRGBCalibrated')
    % Call lum2rgb() function to use lut based conversion of luminance image
    % to RGB image:
    varargout{1} = lum2calrgb(varargin{:});
    
    return;
end

if strcmpi(cmd, 'SwitchMode')
    % Switch VideoSwitcher between monochrome and color mode by sending a
    % sequence of frames with special green-channel control codes:
    if nargin < 3
        error('At least one of the two required input arguments is missing!');
    end
    
    screenid = varargin{1};
    if ~ismember(screenid, Screen('Screens'))
        error('Invalid screenId provided - No such display screen!');
    end

    enableLuminanceMode = varargin{2};
    if isempty(enableLuminanceMode) || ~isscalar(enableLuminanceMode) || ~isnumeric(enableLuminanceMode) || ~ismember(enableLuminanceMode, [0,1])
        error('Invalid enableLuminanceMode flag provided. Must be 0 or 1!');
    end

    if nargin < 4
        % No type parameter provided: Assume box for now:
        VideoSwitcherIsABox = 1;
    else
        VideoSwitcherIsABox = varargin{3};
        if isempty(VideoSwitcherIsABox) || ~isscalar(VideoSwitcherIsABox) || ~isnumeric(VideoSwitcherIsABox) || ~ismember(VideoSwitcherIsABox, [0,1])
            error('Invalid VideoSwitcherIsABox flag provided. Must be 0 or 1!');
        end
    end
    
    % Enumerate all onscreen windows for screen 'screenid':
    allWindows = Screen('Windows');
    allWindowKinds = Screen('WindowKind', allWindows);
    onscreenWins = allWindows(allWindowKinds == 1);
    for win = onscreenWins
        % Is 'win' an onscreen window on screen screenid?
        if Screen('WindowScreenNumber', win) == screenid
            % This 'win'dow is an onscreen window on our target screenid.
            % Close it -- We don't want any onscreen windows on the screen
            % to switch, as they could clash/interfere with the switching
            % operation. Recycling them is impossible as we don't know
            % their configuration wrt. imaging mode, size etc.:
            Screen('Close', win);
        end
    end
    
    % Ok, our target screen 'screenid' is clear of any onscreen windows.
    % Create our own one: Fullscreen, black background clear color, no
    % imaging pipe or anything else, double-buffered. Disable all
    % sync-tests, warnings etc. for this switch action:
    oldverbosity = Screen('Preference', 'Verbosity', 1);
    oldsynclevel = Screen('Preference', 'SkipSyncTests', 2);
    oldvisuallevel = Screen('Preference', 'VisualDebugLevel', 0);
    win = Screen('OpenWindow', screenid, 0, []);
    
    % Hide mouse cursor:
    HideCursor;
    
    % Get current display size:
    [width, height]=Screen('WindowSize', win);

    % Get video refresh duration: We query nominal framerate, as this is faster
    % and sufficient for our purpose:
    ifi=Screen('NominalFramerate', win);
    if ifi == 0
        % Special case: Invalid framerate -- This usually means 60 Hz:
        ifi = 1/60;
    else
        % ifi is 1/Hz:
        ifi = 1/ifi;
    end

    if VideoSwitcherIsABox
        % Perform switching procedure for external box based VideoSwitcher:
        switchColorBox(win, width, height, ifi, enableLuminanceMode);
    else
        % Perform switching procedure for internal PCI based VideoSwitcher:
        switchColorCard(win, width, height, ifi, enableLuminanceMode);
    end
    
    % Show mouse cursor again:
    ShowCursor;

    % Close our window:
    Screen('Close', win);
    
    % Restore old settings:
    Screen('Preference', 'Verbosity', oldverbosity);
    Screen('Preference', 'SkipSyncTests', oldsynclevel);
    Screen('Preference', 'VisualDebugLevel', oldvisuallevel);
    
    return;
end

if strcmpi(cmd, 'SetTrigger')
    error('Trigger facilities not yet implemented. Sorry.');
    return;
end

% Unknown command string - No match to any of the routines above:
error('Unknown subcommand provided!');

return; %#ok<UNRCH>

% Helper functions:
%
% Convert luminance matrix into RGB image to drive a video switcher.
%
% Usage: RGBimage = lum2rgb (lum, ratio [, trigger])
%  Inputs:
%     lum: luminance (MxN matrix with values from 0 to 1)
%     ratio: blue to red ratio of the video switcher
%     trigger: when non-zero, a trigger will be sent in current frame
%         trigger=1 or 'auto', the first line of image
%         trigger=2 or 'top',  the first line with non-zero image
%         trigger=3 or 'middle', the middle line of image
%     If you omit ratio, you should give it in this code
%  Output: RGB image (MxNx3 matrix with values from 0 to 255)
% XL, 01/03/2008
function img = lum2rgb (lum, ratio, trigger)
if nargin<3 || isempty(trigger), trigger=0; end
if nargin<2 || isempty(ratio), ratio=128; end
if nargin<1, disp('Usage: RGBimage = lum2rgb (lum, ratio [, trigger])'); return; end

sz=size(lum);
img = uint8(zeros(sz(1),sz(2),3));   % preallocate memory
lum=lum*255;	% now 0 to 255
img(:,:,3) = uint8(min(255,floor((ratio+1)/ratio*lum)));       % first calculates BLUE
img(:,:,1) = uint8((ratio+1)*lum-double(img(:,:,3))*ratio);	% then remainder is achieved by RED

if trigger==0, return; end

switch trigger
    case {2, 'top'}
        line = 1;    % first line of the image
    case {3, 'middle'}
        line = round(sz(1)/2);    % middle of image
    otherwise
        line = ceil(find(lum',1,'first')/sz(2));    % the first line where lum is non-zero
end
img(line,:,2) = uint8(255);         % set green to 255. Will send trigger when displayed
return;

% Convert luminance matrix into RGB image to drive a video switcher.
% This uses the calibrated luminance table saved in calibratedlum.mat.
% For detail, check the paper http://lobes.usc.edu/Journals/JNM03.pdf
% 
% The steps to get the table:
%  1. Switch the video switcher to grayscale mode;
%  2. Set up the equipement to accurately measure screen luminance;
%     you can use a photometer or data acquisition system;
%  3. Measure 257 luminance levels at RGB of [0 0 b] and [btrr 0 255],
%     where b is 0:255, and btrr the blue to red ratio of switcher; 
%  4. Store 257 luminance in variable callum and normalize them:
%     callum=callum/callum(257);
%  5. Save it under a proper folder with Matlab path: 
%     save calibratedlum callum;
% 
% Usage: RGBimage = lum2calrgb (lum, ratio [, trigger])
%  Inputs:
%     lum: luminance (MxN matrix with values from 0 to 1)
%     ratio: blue to red ratio of the video switcher
%     trigger: when non-zero, a trigger will be sent in current frame
%         trigger=1 or 'auto', the first line of image
%         trigger=2 or 'top',  the first line with non-zero image
%         trigger=3 or 'middle', the middle line of image
%     If you omit ratio, you should give it in this code
%  Output: RGB image (MxNx3 matrix with values from 0 to 255)
% 
% Note: this runs slow when lum matrix is large. 200x200 can take 1 second.
% XL, 01/08/2008
function img = lum2calrgb (lum, ratio, trigger)
if nargin<3 | isempty(ratio), trigger=0; end
if nargin<2 | isempty(ratio), ratio=128; end
if nargin<1, disp('Usage: RGBimage = lum2calrgb (lum, ratio [, trigger])'); return; end

persistent callum; %#ok<USENS>
if isempty(callum) % load only for the first time call
    load('calibratedlum.mat');
    if length(callum)~=257
        error('Calibrated luminance table must have 257 entries.');
    end
end

sz=size(lum);
blue=ones(sz)*255;
for i=1:256
    ind=find(lum>=callum(i) & lum<callum(i+1));
    blue(ind)=i-1; %#ok<FNDSB>
end
img=zeros(sz(1),sz(2),3);   % preallocate memory
img(:,:,3)=blue;
img(:,:,1)=(lum-callum(blue+1))./(callum(blue+2)-callum(blue+1))*ratio;  % red
img=uint8(img);

if trigger==0, return; end
switch trigger
    case {2, 'top'}
        line = 1;    % first line of the image
    case {3, 'middle'}
        line = round(sz(1)/2);    % middle of image
    otherwise
        line = ceil(find(lum',1,'first')/sz(2));    % the first line where lum is non-zero
end
img(line,:,2) = uint8(255);         % set green to 255. Will send trigger when displayed

return;

% Switch between normal color mode and high grayscale mode of a box video switcher. 
% Usage for box switcher:
%  switchColor(windowOrScreenNumber) to switch back and forth between two modes.
% Call it to switch to grayscale mode after you open a window, and 
% call it again to switch back to normal color mode after you finish display.
% XL, 01/08/2008
function switchColorBox(w, width, height, ifi, enableLuminanceMode) %#ok<INUSD>

% Doesn't help doesn't hurt either:
Screen('FillRect',w,0);

ppb=1/ifi*width*height/4e6; % pixels per bit based on 4 MHz detection frequency

% these 2 signals will make a switch
bits=[1 1 1 0 0 1 0 1 0 1 0 1 1 1 1 0 0 0 1 1 0 1 1 0 1; ... 
      1 1 0 1 1 0 1 1 1 0 1 1 0 1 1 0 1 0 1 0 1 1 0 0 1]; 

% factor means the percent of time to display pixels within a frame.
% Here we use a large range of factor so it will work for different settings.
range=[0.6 0.85];   % you can narrow the range at specific monitor setting
step=0.2/ppb; % in case of switch failure, reduce 0.2 to about 0.15
pixelPos=round((0:25)*ppb/range(1));
img1=zeros(1, pixelPos(26), 3); % one line image, will change green layer
img2=img1;
for i=1:25
    img1(1, pixelPos(i)+1:pixelPos(i+1), 2)=bits(1,i)*255;
    img2(1, pixelPos(i)+1:pixelPos(i+1), 2)=bits(2,i)*255;
end
tex(1)=Screen('MakeTexture',w,img1);
tex(2)=Screen('MakeTexture',w,img2);

for i=1:2
    for factor=(range(1):step:range(2))/range(1)
        rd=rand*ppb*0.5; % random shift up to half a bit
        Screen('DrawTexture',w, tex(i),[],[rd 0 pixelPos(26)/factor+rd 1]); 
        Screen('Flip',w);
    end
end

% Release textures:
Screen('Close', tex);

return;

% Switch between normal color mode and high grayscale mode of a card video switcher. 
% Usage: switchColorCard [(color,windowOrScreenNumber)]
%   Optional inputs: 
%     color: 0, 'BW' or 'gray' to switch to high grayscale monochrome mode, 
%        otherwise (or omit) to switch to color mode.
%     windowOrScreenNumber: windowPtr or screen number. Default is current onscreen.
% XL, 11/2007

function switchColorCard(w, width, height, ifi, enableLuminanceMode)

Screen('FillRect',w,0);

ppb=1/ifi*width*height/4e6; % pixels per bit based on 4 MHz detection frequency

% 1st row to gray, 2nd color
bits=[1 1 1 0 0 1 0 1 0 1 0 1 1 1 1 0 0 0 1 1 0 1 1 0 1; ... 
      1 1 0 1 1 0 1 1 1 0 1 1 0 1 1 0 1 0 1 0 1 1 0 0 1]; 

if enableLuminanceMode==1
    % Switch to monochrome mode:
    color=1;
else
    % Switch to RGB mode:
    color=2;
end

% factor means the percent of time to display pixels within a frame.
% Here we use a large range of factor so it will work for different settings.
range=[0.6 0.85];   % you can narrow the range at specific monitor setting
step=0.2/ppb; % in case of switch failure, reduce 0.2 to about 0.15
pixelPos=round((0:25)*ppb/range(1));
img=zeros(1, pixelPos(26), 3); % one line image, will change green layer
for i=1:25
    img(1, pixelPos(i)+1:pixelPos(i+1), 2)=bits(color,i)*255;
end
tex=Screen('MakeTexture',w,img);

for factor=(range(1):step:range(2))/range(1)
    rd=rand*ppb*0.5; % random shift up to half a bit
    Screen('DrawTexture',w,tex,[],[rd 0 rd+pixelPos(26)/factor 1]);
    Screen('Flip',w); 
end

% MK: Release textures:
Screen('Close', tex);

return;
