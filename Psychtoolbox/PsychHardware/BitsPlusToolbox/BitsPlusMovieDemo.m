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

clear all; close all

% Define screen
whichScreen=max(Screen('Screens'));

% Create a sinusoidal image in a Matlab matrix. The grating is
% scaled between 1 and 255. The creation happens in two steps. First
% create a sinusoidal vector, then replicate this vector to produce a
% sinusoidal image. The replication is done by an outer product. This is
% not the fastest way to do it in Matlab, but it is a little clearer.
nPixels = 256;
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

% Open up the a window on the screen & initialize the clut.
% This is done by calling the Screen function of the Psychophysics
% Toolbox. We need to open the onscreen window first, so that
% the offscreen storage allocated subsequently is properly
% matched to the onscreen frame buffer.
[window,screenRect] = Screen(whichScreen,'OpenWindow',128,[],32);
Screen('LoadClut', window, (0:255)'*ones(1,3));

% Generate a clut for the background and use USB interface
% to write it into Bits++.
offClut = ones(256,3)*(2^14-1)/2;
BitsPlusSetClut(window,offClut);

% Magic rect for writing the clut into the frame buffer.
bitsPlusRect = [0     0   524     1];

% Draw grating onto the screen using PsychToolbox.
% We don't see it at this stage becuase we've got
% a uniform clut in the Bits++.
Screen(window,'PutImage',sinImage); 
 
% Loop through, compute clut, push into created offscreen memory.
for i = 1:nCluts
	if (rem(i,10) == 0)
		fprintf('%g ',i);
	end
	contrast = theContrasts(i);
	lowValDev = (1-contrast)/2;
	highValDev = (1+contrast)/2;
	clutEntries = round(linspace(lowValDev*(2^14-1),highValDev*(2^14-1),256)');
	theCluts(:,:,i) = [clutEntries clutEntries clutEntries];
	bitsPlusClutRow(i,:,:) = BitsPlusEncodeClutRow(theCluts(:,:,i));  % converts to 1X524X3
	[w(i),temp]=Screen(window,'OpenOffscreenWindow',[],bitsPlusRect);
	Screen(w(i),'PutImage',bitsPlusClutRow(i,:,:),temp);	% draw value of CLUT into offscreen windows
end
fprintf('. Done.\n');				

% Animate the clut.  The timing works a little different from
% the way we think about a conventional clut.  We want to write
% the clut into the first line of the frame buffer after the raster
% has gone by, so that we don't get half of one clut and half of
% another.  The new clut won't latch into the Bits++ until the
% raster goes by the first line of the frame buffer, i.e. at the
% start of the next frame.  So the timing sequence is a) wait
% for blanking, b) delay a suitable amount so that the raster
% has gone by the first line, c) write in the new clut.  This clut
% then takes effect on the next frame, not the one where we wrote it.
% The parameter rasterDelay below determines what we mean by "suitable
% amount."  Also, we need a last call to WaitBlanking at the end of the
% loop so that we don't truncate the last frame.
nCycles=8;
clutCounter = 1;
rasterDelay = 0.003;
s0=0;f0=0;f=0;s=0;rem(1,1);
Screen(window,'WaitBlanking'); WaitSecs(rasterDelay);
[f0,s0]=Screen(window,'PeekBlanking');
for i=1:nCluts*nCycles
	Screen('CopyWindow',w(clutCounter),window,bitsPlusRect,bitsPlusRect);
	Screen(window,'WaitBlanking'); WaitSecs(rasterDelay);
	s(i)=GetSecs;
	if (clutCounter == nCluts); clutCounter = 1; else; clutCounter = clutCounter +1; end
end
s=diff(s);
frames1=sum(s)*FrameRate(whichScreen)-length(s);
Screen(window,'WaitBlanking'); WaitSecs(rasterDelay);
[f,s]=Screen(window,'PeekBlanking');

% Write the off clut back in so we end with a uniform field
WaitSecs(1);
f=f-f0;
s=s-s0;
clutsShown=nCluts*nCycles;

% Close the window.
Screen(window,'Close');

% Set the Bits++ clut to linear ramp
BitsPlusBlank(whichScreen);


