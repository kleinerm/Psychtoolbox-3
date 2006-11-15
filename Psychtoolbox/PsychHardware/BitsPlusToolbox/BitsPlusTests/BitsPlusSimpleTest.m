% BItsSimpleTest
%
% Simple test of Bits++ interface in normal mode.  Writes CLUT
% frame buffer, so this is how Bits++æshould be configured to
% accept it.
%
% 9/13/02 dhb
% 2/26/03 dhb  Force on-card CLUT to be a identity mapping.
% 9/20/03 dhb  No more calls to MEX file.

% Define screen
whichScreen = 1;

% Open window fill frame buffer with zeros
[window, screenRect] = Screen('OpenWindow', whichScreen, 0, [], 32);
LoadIdentityClut(window, 1);

% Use BITS++ to set uniform lookup tables of increasing values.
% Hit key to proceed through. Screen intensity should increase
% monotonically.
for colorval = round(linspace(0,2^14-1,20))
	uniclut = colorval*ones(256,3)/255;
	fprintf('Setting to value %g\n',colorval);
	BitsPlusSetClut(window, uniclut);
	GetChar;
end

% Close the window.
Screen(window,'Close');

% Blank the screen
BitsPlusBlank(whichScreen);
