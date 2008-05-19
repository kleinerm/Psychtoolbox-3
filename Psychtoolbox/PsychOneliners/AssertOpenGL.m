function AssertOpenGL
% AssertOpenGL
%
% Break and issue an eror message if the installed Psychtoolbox is not
% based on OpenGL or Screen() is not working properly.
% To date there are four versions of the Psychtoolbox, each based on a
% different graphics library:
%
%  OS9: Psychtoolbox-2, based on Apple's QuickDraw.
%  Win: Psychtoolbox-2, based on Direct X and GDI.
%  Win: OpenGL for the ported OSX-Psychtoolbox, aka Psychtoolbox-3.
%  OSX: OpenGL for Psychtoolbox-3.
%  Linux: OpenGL for Psychtoolbox-3.
%
%  The Psychtoolboxes based on OpenGL are partially incompatible (see below)
%  with previous Psychtoolboxes.  A script which relies on the OpenGL
%  Psychtoolbox should call AssertOpenGL so that it will issue the
%  appropriate warning if a user tries to run it on a computer with a
%  non-OpenGL based Psychtoolbox installed.
%
%  OpenGL-based Psychtoolboxes are distinguised by the availability of these
%  functions:
%
%   Screen('Flip',...);
%   Screen('MakeTexture');
%   Screen('DrawTexture');
%
%
%  If you know you're using Psychtoolbox-3, then the most likely cause for
%  this error message is a problem in the configuration of your
%  Matlab+System setup, or in the Psychtoolbox installation.
%
%  Typically either the Screen MEX file can't be found or accessed, due to
%  some wrong Matlab path (Proper Screen file not in Matlab path), or due
%  to some permission issue (insufficient security access permissions -
%  typically found on MS-Windows systems), or the Screen MEX file can't be
%  loaded and initialized by Matlab due to some missing or wrong system
%  library on your machine, e.g., the C runtime library is of an
%  incompatible type. Simply type the command "Screen" at the Matlab prompt
%  to see if this may be an issue.
%
%  In both cases, indicated by some "file not found" or "file could not by
%  accesses" or "invalid MEX file" error message by Matlab, you may want to
%  run the SetupPsychtoolbox command again. This will either fix the
%  problem for you by reconfiguring Psychtoolbox, or it will provide more
%  diagnostic error and troubleshooting messages. Make also sure that you
%  read the troubleshooting tips in the "Download" and "Frequently asked
%  questions" sections of our Psychtoolbox Wiki at
%  http://www.psychtoolbox.org
%
% See also: IsOSX, IsOS9 , IsWin, IsLinux.

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
   value=Screen('Preference', 'SkipSyncTests'); %#ok<NASGU>
   return;
catch
   % Tried to execute old Screen command of old Win-PTB or MacOS9-PTB. This will tell user about non-OpenGL PTB.
   error('This script or function is designated to run only an Psychtoolbox based on OpenGL. Read "help  AssertOpenGL" for more info.')   
end;
