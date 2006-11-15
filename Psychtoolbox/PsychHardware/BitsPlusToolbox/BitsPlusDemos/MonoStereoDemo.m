% MonoStereoDemo
%
% demonstration of displaying a precalculated texture based movie, whilst
% controlling the FE1 goggles using a Tlock packet.
%
% For use with the OSX version of Psychtoolbox.
% tested with version 1.0.5.
% uses the hidden 'LoadNormalizedGammaTable' screen function.
%
% Please note that this demo as configured needs 256MB system ram.
% It also needs the Bits++ from Cambridge Research Systems!!
% Designed to be used in the "mono++" enhanced monochrome mode.
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
Screen('FillRect',window, gray);
Screen('Flip', window);
Screen('FillRect',window, gray);

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

% textures can't seem to be bigger than the screen, though they have to be
% square. So the max texture size is the smaller of the screen dimensions!!
max_texture_size = min([screenWidth screenHeight]);

% pre calculate two counter rotating gratings

% some graphics cards have a hardware texture size limit of 256 square
% (any bigger and it is slow). Be aware that textures use up RAM very quickly.
texture_size=256;

if (max_texture_size < texture_size)
    error('screen must have a minimum resolution of 256');
end    

image=zeros(texture_size,texture_size,3);
[x,y] = meshgrid(1: texture_size, 1: texture_size);

% number of cycles of sine wave in patch
cycles=10;
 

for orient=1:180
    fprintf('Calculating angle %d\n',orient);
    phase=cycles*2*pi*((sin(pi*(orient-1)/180)*(y - texture_size/2)+cos(pi*(orient-1)/180)*(x - texture_size/2)) - 1)/(texture_size/2);
    mono=0.5*(2^16)*(sin(phase)+1);
    
    % ==================================================================
    % CODE NEEDED HERE !
    % "mono" should be corrected here for the inverse characteristic of
    % the monitor.
    % ==================================================================    
    
    image = BitsPlusPackMonoImage(mono);
  
    left(orient)= Screen('MakeTexture',whichScreen,image); 

    phase=cycles*2*pi*((-sin(pi*(orient-1)/180)*(y - texture_size/2)+cos(pi*(orient-1)/180)*(x - texture_size/2)) - 1)/(texture_size/2);
    mono=0.5*(2^16)*(sin(phase)+1);
    
    % ==================================================================
    % CODE NEEDED HERE !
    % "mono" should be corrected here for the inverse characteristic of
    % the monitor.
    % ==================================================================    
    
    image = BitsPlusPackMonoImage(mono);
    
    right(orient)= Screen('MakeTexture',whichScreen,image); 
end  

% now quickly display the previously calculated textures,
% add in the control for the goggles

fprintf('Displaying counter-rotating gratings, hold a key to exit \n');

while(~KbCheck)
    for orient= 1:180
        Screen('DrawTexture',window,left(orient));
        % bitsgoggles adds the one line image to control the digital o/p
        % then calls 'Flip'
        % Note that bitsGoggles controls the goggle state for the frame
        % >after< it is displayed, hence we set the right goggle open now
        bitsGoggles(1,0,window);
    
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
    % restore the Mono++ overlay LUT to a linear ramp
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
    % Blank the screen (zero the Mono++ overlay LUT),
    % - doesn't actually quite blank the screen.
    BitsPlusSetClut(window,zeros(256,3));

    % draw a black background on front and back buffers to clear out any old LUTs
    Screen('FillRect',window, black);
    Screen('Flip', window);
    Screen('FillRect',window, black);
    Screen('Flip', window);

    Screen('CloseAll');
end



