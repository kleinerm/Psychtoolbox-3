function AssertOpenGL
% AssertOpenGL
%
% Break and issue an eror message if the installed Psychtoolbox is not
% based on OpenGL.  To date there are four versions of the Psychtoolbox,
% each based on a different graphics library:
%
%  OS9: QuickDraw
%  Win: Direct X and GDI for the old Windows Psychtoolbox.
%  Win: OpenGL for the ported OSX-Psychtoolbox.
%  OSX: OpenGL
%
%  The Psychtoolboxes based on OpenGL are partially incompatible (see below)
%  with previous Psychtoolboxes.  A script which relies on the OpenGL
%  Psychtoolbox should call AssertOpenGL so that it will issue the
%  appropriate warning if a user tries to run it on a computer with a
%  non-OpenGL based Psychtoolbox installed.   Because future versions of the
%  Psychtoolbox on Windows will also be based on  OpenGL, your scripts written
%  for the OS X psychtoolbox and OpenGL should test for compatability with the
%  installed Psychtoolbox by using AssertOpenGL instead of by testing
%  for OS X.
%
%  OpenGL-based Psychtoolboxes are distinguised by the availability of these
%  functions:
%
%   Screen('Flip',...);
%   Screen('MakeTexture');
%   Screen('DrawTexture');
%
% and the absence of these:
%
%   Screen('WaitBlanking',...);
%   Screen('CopyWindow');
%
% See also: IsOSX, IsOS9 , IsWin

% HISTORY
% 7/10/04   awi     wrote it.
% 7/13/04   awi     Fixed documentation.
% 10/6/05   awi	  Note here cosmetic changes by dgp between 7/13/04 and 10/6/05
% 12/31/05  mk      Detection code modified to really query type of Screen command (OpenGL?)
%                   instead of OS type, as PTB-OpenGL is now available for Windows as well.

% We put the detection code into a try-catch-end statement: The old Screen command on windows
% doesn't have a 'Version' subfunction, so it would exit to Matlab with an error.
% We catch this error in the catch-branch and output the "non-OpenGL" error message...
try
   % Query a Screen subfunction that only exists in the new Screen command If this is not
   % OpenGL PTB,we will get thrown into the catch-branch...
   value=Screen('Preference', 'SkipSyncTests');
   return;
catch
   % Tried to execute old Screen command of old Win-PTB or MacOS9-PTB. This will tell user about non-OpenGL PTB.
   error('This script or function is designated to run only an Psychtoolbox based on OpenGL.  See AssertOpenGL.')   
end;
