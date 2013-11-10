% BitsPlusColorImageTest
% 
% Use of Color++ mode.
%
% 8/8/04	dhb		Started it.
% 18/4/05   ejw     Converted it to run with OSX version of Psychtoolbox
% 10/24/13  dhb     Pass scaling args to packing routine and truncate test image.
%                   These were needed to make this work with my Bits++ in Color++ mode
%                   today.  I think the cropping has to do with the spatial res of my
%                   monitor.  How this ever worked without the scaling args, though,
%                   is unclear to me.

% Clear
clear all;

% Define screen
whichScreen=max(Screen('Screens'));

% First need an interesting test image.
% Load it in.  This is an RGB image with
% data in range 0-1.  
fprintf('Loading a high dynamic range test image\n');
load colorPlusTest

fprintf('Converting to color++ format\n');
theImage = theImage(1:512,1:512,:);
packedImage = BitsPlusPackColorImage(theImage,1,1);
%packedImage = BitsPlusPackColorImage(temp1,0,0);

% Show the image
[m,n,p] = size(packedImage);
rect = [0 0 m n];
fprintf('Showing image\n');
% Open a double buffered fullscreen window
% does not like opening it 24 bit deep, so stick with 32 bits,
% though we don't want alpha!!
[window,screenRect] = Screen('OpenWindow',whichScreen,0,[],32,2);

% find out how big the window is
[screenWidth, screenHeight]=Screen('WindowSize', window);

% textures can't seem to be bigger than the screen, though they have to be
% square. So the max texture size is the smaller of the screen dimensions!!
max_texture_size = min([screenWidth screenHeight]);

% Find the color values which correspond to white and black.  Though on OS
% X we currently only support true color and thus, for scalar color
% arguments,
% black is always 0 and white 255, this rule is not true on other platforms will
% not remain true on OS X after we add other color depth modes.  
white=WhiteIndex(whichScreen);
black=BlackIndex(whichScreen);
gray=(white+black)/2;
if round(gray)==white
	gray=black;
end

% the following is not necessary, it just stops you staring at a blank
% screen if you are not in colour++ mode, and have a black LUT loaded into
% Bits++ :<(
%   restore the Bits++ LUT to a linear ramp
linear_lut =  repmat(round(linspace(0, 2^16 -1, 256))', 1, 3);
BitsPlusSetClut(window,linear_lut);

% draw a gray background on front and back buffers
Screen('FillRect',window, black);
Screen('Flip', window);
Screen('FillRect',window, black);

% make sure the graphics card LUT is set to a linear ramp
% (else the encoded data will not be recognised by Bits++).
% There is a bug with the underlying OpenGL function, hence the scaling 0 to 255/256.   
Screen('LoadNormalizedGammaTable',window,linspace(0,(255/256),256)'*ones(1,3));

%textureIndex= Screen('MakeTexture',whichScreen,packedImage);
%Screen('DrawTexture',window,textureIndex);

Screen('PutImage',window,packedImage);
Screen(window,'Flip');

fprintf('Hit any character to continue\n');

% GetChar causes problems so replace with KbWait
%GetChar;
KbWait;

% if the system only has one screen, set the LUT in Bits++ to a linear ramp
% if the system has two or more screens, then blank the screen.
if (whichScreen == 0)
    % restore the Bits++ LUT to a linear ramp
    linear_lut =  repmat(round(linspace(0, 2^16 -1, 256))', 1, 3);
    BitsPlusSetClut(window,linear_lut);
    
    % draw a gray background on front and back buffers to clear out any old LUTs
    Screen('FillRect',window, gray);
    Screen('Flip', window);
    Screen('FillRect',window, gray);
    Screen('Flip', window);

    % Close the window.
    Screen('CloseAll');    
else
    % Blank the screen
    BitsPlusSetClut(window,zeros(256,3));

    % draw a black background on front and back buffers to clear out any old LUTs
    Screen('FillRect',window, black);
    Screen('Flip', window);
    Screen('FillRect',window, black);
    Screen('Flip', window);

    Screen('CloseAll');
end 	 

	
