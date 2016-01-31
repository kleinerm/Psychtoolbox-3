% BItsPlusMonoTest
%
% Simple test of Bits++ interface when it is in its Mono++
% mode.
%
% 2/15/04  dhb	Wrote it.

% Define screen
whichScreen=1;

% Open window fill frame buffer with zeros
[window,screenRect] = Screen(whichScreen,'OpenWindow',0);
SCREEN(window,'SetClut',(0:255)'*ones(1,3));

% Put up an image at various intensities using r/g plane packing.
% This uses the direct map features of the Mono++ mode.
for colorval = round(linspace(0,2^14-1,20))
  monoImage = BitsPlusPackMonoImage(colorval*ones(300,300));
  fprintf('Image setting to value %g\n',colorval);
  SCREEN(window,'PutImage',monoImage);
  KbStrokeWait;
end

% Use BITS++ to set uniform lookup tables of increasing values.
% Hit key to proceed through. Screen intensity should increase
% monotonically.
theImage = zeros(300,300,3);
theImage(:,:,3) = ones(300,300);
SCREEN(window,'PutImage',theImage);
for colorval = round(linspace(0,2^14-1,20))
  uniclut = colorval*ones(256,3);
  fprintf('Clut setting to value %g\n',colorval);
  BitsPlusSetClut(window,uniclut);
  KbStrokeWait;
end

% Close the window.
Screen(window,'Close');

% Blank the screen
BitsPlusBlank(whichScreen);
