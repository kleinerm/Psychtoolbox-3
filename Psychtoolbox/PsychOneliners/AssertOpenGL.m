function AssertOpenGL

% AssertOpenGL
%
% OSX and OS9: ___________________________________________________________________
%
% Break and issue an eror message if the installed Psychtoolbox is not
% based on OpenGL.  To date there are three versions of the Psychtoolbox,
% each based on a different graphics library:
%
%  OS9: QuickDraw
%  Win: Direct X and GDI
%  OSX: OpenGL
%
%  The Psychtoolboxes based on OpenGL are incompatible (see below)
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
% WIN: ________________________________________________________________
% 
% AssertOpenGL does not yet exist in Windows.
% 
% _________________________________________________________________________
%
% See also: IsOSX, IsOS9 , IsWin

% HISTORY
% 7/10/04   awi     wrote it.
% 7/13/04   awi     Fixed documentation.
% 10/6/05   awi		Note here cosmetic changes by dgp between 7/13/04 and 10/6/05

if ~IsOSX 
    error('This script or function is designated to run only an Psychtoolbox based on OpenGL.  See AssertOpenGL.');
end
