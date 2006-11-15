% BitsLutStereoDemo
%
% demonstration of displaying a precalculated texture based movie, whilst
% separately animating the LUT, and whilst
% controlling the FE1 goggles using a Tlock packet.
%
% For use with the OSX version of Psychtoolbox.
% tested with version 1.0.5, Matlab release 14 sp 2 and OSX 10.3.9
% uses the hidden 'LoadNormalizedGammaTable' screen function.
%
% Please note that this demo needs the Bits++ from Cambridge Research Systems!!
%
% Sections have been lifted from the OSX movie demo etc.
%
% 26/04/2005    ejw     wrote it.

clear all; close all

% Define screen
whichScreen=max(Screen('Screens'));

% Find the color values which correspond to white and black.  Though on OS
% X we currently only support true color and thus, for scalar color
% arguments,
% black is always 0 and white 255, this rule is not true on other platforms will
% not remain true on OS X after we add other color depth modes.  
white=WhiteIndex(whichScreen);
black=BlackIndex(whichScreen);
gray=GrayIndex(whichScreen);

% Open up the a window on the screen.
% This is done by calling the Screen function of the Psychophysics
% Toolbox. We need to open the onscreen window first, so that
% the offscreen storage allocated subsequently is properly
% matched to the onscreen frame buffer.
[window,screenRect] = Screen('OpenWindow',whichScreen,128,[],32,2);

% THE FOLLOWING STEP IS IMPORTANT.
% make sure the graphics card LUT is set to a linear ramp
% (else the encoded data will not be recognised by Bits++).
% There is a bug with the underlying OpenGL function, hence the scaling 0 to 255/256.  
% This demo will not work using a default gamma table in the graphics card,
% or even if you set the gamma to 1.0, due to this bug.
% This is NOT a bug with Psychtoolbox!
Screen('LoadNormalizedGammaTable',window,linspace(0,(255/256),256)'*ones(1,3));

% draw a gray background on front and back buffers
%Screen('FillRect',window, gray);
%Screen('Flip', window);
%Screen('FillRect',window, gray);

% =================================================================
% CODE NEEDED HERE !
% "linear_lut" should be replaced here with one giving the inverse
% characteristic of the monitor.
% =================================================================
% restore the Bits++ LUT to a linear ramp
linear_lut =  repmat(round(linspace(0, 2^16 -1, 256))', 1, 3);
BitsPlusSetClut(window,linear_lut);

% find out how big the window is
[screenWidth, screenHeight]=Screen('WindowSize', window);

[y,x] = meshgrid(-(screenHeight - 1)/2: (screenHeight - 1)/2, -(screenWidth - 1)/2: (screenWidth - 1)/2);

blend_image = sqrt((exp(-abs(5*x/((screenWidth - 1)/2))).^2) + (exp(-abs(5*y/((screenHeight - 1)/2))).^2));

% encode the image in mono++ format
encoded_image = BitsPlusPackMonoImage(blend_image*((2^16)-1));

Screen('PutImage', window, encoded_image);
Screen(window,'Flip');

Screen('PutImage', window, encoded_image);

% textures can't seem to be bigger than the screen, though they have to be
% square. So the max texture size is the smaller of the screen dimensions!!
max_texture_size = min([screenWidth screenHeight]);

% pre calculate two counter rotating gratings

% some graphics cards have a hardware texture size limit of 256 square
% (any bigger and it is slow). Be aware that textures use up RAM very quickly.
texture_size=180;   % the diagonal dimension of 180 x 180 is 255, which is our
                    % maximum number of LUT entries

if (max_texture_size < texture_size)
    error('screen must have a minimum resolution of 256');
end 

% check that the screen width is at least 524 pixels
if (screenWidth < 524)
    error('window is not big enough to encode the Bits++ CLUT');
end 

image=zeros(texture_size,texture_size,3);
[x,y] = meshgrid(-(texture_size - 1)/2: (texture_size - 1)/2, -(texture_size - 1)/2: (texture_size - 1)/2);

% number of cycles of sine wave in patch
speed=10/359;
cycles_across=5;
 
% unfortunately we have to iterate this, to generate the textures??
for orient=0:359
    fprintf('Calculating angle %d\n',orient);
    
    phase = round(128 + sin(pi*orient/180)*y + cos(pi*orient/180)*x);
    
    image(:,:,1) = 0;
    image(:,:,2) = 0;
    image(:,:,3) = phase;     
  
    left(orient + 1)= Screen('MakeTexture',whichScreen,image); 

    phase = round(128 - sin(pi*orient/180)*y + cos(pi*orient/180)*x);
    
    image(:,:,1) = 0;
    image(:,:,2) = 0;
    image(:,:,3) = phase; 
    
    right(orient + 1)= Screen('MakeTexture',whichScreen,image); 
end 

% create some drifting sinewave LUTs
[index,offset,gun] = meshgrid(0: 255, 0: 359, 1: 3);

% =================================================================
% CODE NEEDED HERE !
% the code needs to apply the inverse characteristic of the monitor.
% =================================================================

luts = (2^16 - 1) * (sin(cycles_across * 2 * pi * (index/255 + (offset*speed))) + 1) / 2;

% now quickly display the previously calculated textures,
% addind the control for the goggles

fprintf('Displaying counter-rotating gratings, hold a key to exit \n');


while(~KbCheck)
    for orient= 1:360
        % encode the LUT
        newClutRow = BitsPlusEncodeClutRow(squeeze(luts(orient,:,:)));

        rect=[0 0 size(newClutRow,2) 1];
        Screen('PutImage', window, newClutRow, rect, rect);       
        
        Screen('DrawTexture',window,left(orient));
        % bitsgoggles adds the one line image to control the digital o/p
        % then calls 'Flip'
        % Note that bitsGoggles controls the goggle state for the frame
        % >after< it is displayed, hence we set the right goggle open now
        bitsGoggles(1,0,window);
        
        % encode the LUT
        newClutRow = BitsPlusEncodeClutRow(squeeze(luts(orient,:,:)));

        rect=[0 0 size(newClutRow,2) 1];
        Screen('PutImage', window, newClutRow, rect, rect);        
    
        Screen('DrawTexture',window,right(orient));
        % bitsgoggles adds the one line image to control the digital o/p
        % then calls 'Flip'
        % Note that bitsGoggles controls the goggle state for the frame
        % >after< it is displayed, hence we set the left goggle open now        
        bitsGoggles(0,1,window);    
    end
end

% reset the digital output data
Mask=0;
Command=0;
Data=zeros(1,248);
bitsEncodeDIO(Mask,Data,Command, window);

% if the system only has one screen, set the LUT in Bits++ to a linear ramp
% if the system has two or more screens, then blank the screen.
if (whichScreen == 0)
    % =================================================================
    % CODE NEEDED HERE !
    % "linear_lut" should be replaced here with one giving the inverse
    % characteristic of the monitor.
    % =================================================================
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



