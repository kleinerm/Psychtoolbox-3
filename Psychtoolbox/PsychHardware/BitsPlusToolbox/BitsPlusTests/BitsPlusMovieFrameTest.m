% BitsPlusMovieFrameTest
%
% BitsMovieFrame displays a square that flickers between
% two levels at the frame rate, using lookup
% table animation, using the Bits++ box.
%
% This is a useful stimulus for monitoring with a photodiode
% to verify that the clut animation is running at the frame rate.
%
% The routine also contains some timing checks. On our system, these
% also confirm frame rate operation.

% 9/20/02  dhb  Version created from ClutMovieDemo.
% 10/xx/02 jmh  Fixed it up.
% 11/4/02  dhb, jmh Comments, etc.
% 11/6/02  jmh  Modified BitsPlusMovieDemo to test for lost frames

clear all; close all

% Define screen
whichScreen=1;

% Create a sinusoidal image in a Matlab matrix. The grating is
% scaled between 1 and 255. The creation happens in two steps. First
% create a sinusoidal vector, then replicate this vector to produce a
% sinusoidal image. The replication is done by an outer product. This is
% not the fastest way to do it in Matlab, but it is a little clearer.
Nbits = 14; 
nLevels = 2^Nbits;
bg_RGB = (nLevels/256)*[128 128 128];
Contrast = nLevels/4;
nCluts = 2;
theCluts = zeros(256,3,nCluts);

% Open up the a window on the screen & initialize the clut.
% This is done by calling the Screen function of the Psychophysics
% Toolbox. We need to open the onscreen window first, so that
% the offscreen storage allocated subsequently is properly
% matched to the onscreen frame buffer.
[window,screenRect] = Screen(whichScreen,'OpenWindow',0,[],32);

% Generate a clut for the background and use USB interface
% to write it into Bits++.
offClut = [ones(256,1)*bg_RGB];
BitsPlusSetClut(window,offClut);

% Magic rect for writing the clut into the frame buffer.
bitsPlusRect = [0     0   524     1];

% To look for frame losses, we only need to flip between
% 2 CLUTs. We therefore "manually" generate these 2 CLUTs
Clut1 = offClut; Clut2 = offClut;
Clut1(2,:) = bg_RGB - Contrast*ones;
Clut2(2,:) = bg_RGB + Contrast*ones;
bitsClutRowOff = BitsPlusEncodeClutRow(offClut);  % converts to 1X524X3
bitsClutRow1 = BitsPlusEncodeClutRow(Clut1);  % converts to 1X524X3
bitsClutRow2 = BitsPlusEncodeClutRow(Clut2);  % converts to 1X524X3
[w(1),temp]=Screen(window,'OpenOffscreenWindow',[],bitsPlusRect);
[w(2),temp]=Screen(window,'OpenOffscreenWindow',[],bitsPlusRect);
[w(3),temp]=Screen(window,'OpenOffscreenWindow',[],bitsPlusRect);
Screen(w(1),'PutImage',bitsClutRow1,temp);	% draw value of CLUT into offscreen windows
Screen(w(2),'PutImage',bitsClutRow2,temp);	% draw value of CLUT into offscreen windows
Screen(w(3),'PutImage',bitsClutRowOff,temp);% draw value of CLUT into offscreen windows
fprintf('. Done.\n');

nCluts=2; nCycles= 75;	
sBlit = zeros(1,nCycles*nCluts);		
sBlanking = zeros(1,nCycles*nCluts);
peekCount = zeros(1,nCycles*nCluts);
clutCounter = 1;
rasterDelay = 0.003;
s0=0;f0=0;f=0;s=0;rem(1,1);
targetRect = [0 0 200 200];
targetRect = CenterRect(targetRect,screenRect);
Screen(window,'FillRect',1,targetRect);
Screen(window,'WaitBlanking'); WaitSecs(rasterDelay);
[p0,s0]=Screen(window,'PeekBlanking');
for i=1:nCluts*nCycles
	Screen('CopyWindow',w(clutCounter),window,bitsPlusRect,bitsPlusRect);
	sBlit(i) = GetSecs;
	Screen(window,'WaitBlanking');
	[peekCount(i),sBlanking(i)] = Screen(window,'PeekBlanking');
	WaitSecs(rasterDelay);
	if (clutCounter == nCluts); clutCounter = 1; else; clutCounter = clutCounter +1; end
end

% Write the off clut back in so we end with a uniform field
Screen('CopyWindow',w(3),window,bitsPlusRect,bitsPlusRect);
Screen(window,'WaitBlanking'); WaitSecs(rasterDelay);

% Check that we didn't miss any frames as we played the movie.
peekCount = peekCount-p0;
sBlanking = sBlanking-s0;
sBlit = sBlit-s0;
if (any(diff(peekCount) ~= 1))
	fprintf('Peek count indicates a missed frame\n');
else
	fprintf('Peek count OK\n');
end

% Check that blit always preceeds blanking by enough
proceedTime = sBlanking-sBlit;
headroomTime = 0.003;
if (any(proceedTime < headroomTime))
	fprintf('A blit was too close to a blanking\n');
else
	fprintf('Blits all happened on time\n');
end

% Close the window.
Screen(window,'Close');

% Set the Bits++ clut to linear ramp
BitsPlusBlank(whichScreen);


