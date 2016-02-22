% BitsPlusMovieDemo
%
% BitsPlusMovieDemo displays a counterphase flickering grating, by lookup
% table animation, using the Bits++ box.
%
% Note that you need to precompute the cluts and write them into
% offscreen memory in advance, so that the blits happen fast.  In
% this example, each clut goes into its own offscreen window.  In
% real applications, we have found that the precomputation is considerably
% faster if we allocate one large offscreen window and put each clut
% into a different rect inside of it.

% 9/20/02  dhb  Version created from ClutMovieDemo.
% 10/xx/02 jmh  Fixed it up.
% 11/4/02  dhb, jmh Comments, etc.
% 9/20/03  dhb  No more calls to BitsPlus mex file.
% 18/4/05  ejw  Converted it to run with OSX version of Psychtoolbox

clear; close all

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

% make sure the graphics card LUT is set to a linear ramp
% (else the encoded data will not be recognised by Bits++).
% There is a bug with the underlying OpenGL function, hence the scaling 0 to 255/256.   
Screen('LoadNormalizedGammaTable',window,linspace(0,(255/256),256)'*ones(1,3));

% draw a gray background on front and back buffers
Screen('FillRect',window, gray);
Screen('Flip', window);
Screen('FillRect',window, gray);

% Generate a uniform clut to hide the grating for the moment
offClut = ones(256,3)*(2^16-1)/2;
BitsPlusSetClut(window,offClut);

% find out how big the window is
[screenWidth, screenHeight]=Screen('WindowSize', window);

% textures can't seem to be bigger than the screen, though they have to be
% square. So the max texture size is the smaller of the screen dimensions!!
max_texture_size = min([screenWidth screenHeight]);

% Create a sinusoidal image in a Matlab matrix. The grating is
% scaled between 1 and 255. The creation happens in two steps. First
% create a sinusoidal vector, then replicate this vector to produce a
% sinusoidal image. The replication is done by an outer product. This is
% not the fastest way to do it in Matlab, but it is a little clearer.

% because we are going to borrow the top line of the image to encode a
% LUT, we need to make the image at least 524 pixels square.
nPixels = max_texture_size;
cyclesPerImage = 4;
sinVector = 1+(1+sin(2*pi*cyclesPerImage*(1:nPixels)/nPixels))/2*254;
sinImage = ones(nPixels,1)*sinVector;

% Create a set of color lookup tables (cluts) for animating the
% grating. Each clut starts life as a 256 by three matrix. But
% since we write them into the frame buffer to talk to the Bits++
% box, we then create an offscreen window for each clut and write
% it in.  This needs to be done in advance so that the blit of
% the lookup table is fast. 
%
% The computed set of cluts produces one cycle of counterphase flicker.
% We also create a uniform clut (offClut) to initialize the display.
%
% The variable nCluts determines the number of temporal quantization intervals.
%
% This routine does not do gamma correction -- it's purpose is just
% to show how to interface to the Bits++.
nCluts = 80;
theCluts = zeros(256,3,nCluts);
theContrasts = sin(2*pi*(0:nCluts-1)/nCluts);
fprintf('Computing %g cluts: ',nCluts);

% Draw grating onto the screen using PsychToolbox.
% We don't see it at this stage because we've got
% a uniform clut in the Bits++.
textureIndex= Screen('MakeTexture',whichScreen,sinImage);
Screen('DrawTexture',window,textureIndex);
Screen(window,'Flip');

% Loop through, compute clut, make textures including these LUTs.
for i = 1:nCluts
	if (rem(i,10) == 0)
		fprintf('%g ',i);
	end
	contrast = theContrasts(i);
	lowValDev = (1-contrast)/2;
	highValDev = (1+contrast)/2;
    
    % =====================================================
    % CODE NEEDED HERE !
    % "clutEntries" should be modified to give the inverse
    % characteristic of the monitor.
    % =====================================================    
	clutEntries = round(linspace(lowValDev*(2^16-1),highValDev*(2^16-1),256)');
    
	theCluts(:,:,i) = [clutEntries clutEntries clutEntries];
	bitsPlusClutRow(i,:,:) = BitsPlusEncodeClutRow(theCluts(:,:,i));  % converts to 1X524X3
    
    % copy the encoded LUT to the top line of the texture
    sinImage(1,1:524,1:3) = bitsPlusClutRow(i,:,:);
    
    % create the texture
    w(i)= Screen('MakeTexture',whichScreen,sinImage);    
    
end
fprintf('. Done.\n');

% Animate the clut.
nCycles=8;
clutCounter = 1;
for i=1:nCluts*nCycles
    Screen('DrawTexture',window,w(clutCounter));
   	Screen(window,'Flip');
	if (clutCounter == nCluts); clutCounter = 1; else; clutCounter = clutCounter +1; end
end

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



