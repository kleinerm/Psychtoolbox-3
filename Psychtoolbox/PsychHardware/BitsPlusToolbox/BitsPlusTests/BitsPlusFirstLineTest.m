% BitsPlusFirstLIneTest
%
% Test first-line interface for bitsplus clut.  

% 10/8/02 dhb

whichScreen=max(Screen('Screens'));

% Open window. Fill frame buffer with zeros
[window,screenRect] = Screen(whichScreen,'OpenWindow',0);
Screen(window,'SetClut',(0:255)'*ones(1,3));
defaultClut = Screen(window,'GetClut');

% Use BITS++ to set uniform lookup tables of increasing values.
% Hit key to proceed through. Screen intensity should increase
% monotonically, but does not.  Also note very slow CLUT write time.
for colorval = round(linspace(0,2^14-1,10))
  uniclut = colorval*ones(256,3);
  fprintf('Setting to value %g\n',colorval);
  clutRow = BitsPlusEncodeClutRow(uniclut);
  checkRow = BitsPlusWriteClutRow(window,clutRow);
  checkIt = clutRow-checkRow;
  fprintf('Max diff = %g\n',max(abs(checkIt(:))));
  KbStrokeWait;
end

Screen(window,'Close');
BitsPlusBlank(whichScreen);
