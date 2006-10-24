function ActiveWire
%
% The Psychtoolbox ActiveWire command is used to control the ActiveWire USB 
% device which provides 16 bits of parallel bi-directional digital i/o.  The 
% ActiveWire device is useful for interfacing your PC to other devices
% such as lab equipment or custom-made subject response boxes.  
% See http://www.activewireinc.com/ for more information on the ActiveWire 
% device or to purchase.  
%
% ActiveWire with no arguments lists ActiveWire commands. To obtain help on
% a command follow the name of the command with a question mark.
% For example, to get help on the 'OpenDevice' command at the Matlab prompt
% enter:
% ActiveWire('OpenDevice?')
%
% Microsoft Windows: -------------------------------------------------------
%
%  -You must be using a computer with a USB port.
%  -You must have an ActiveWire device on your computer's USB bus.
%  -Follow the instructions for downloading and installing the ActiveWire 
%   driver.  The instructions can be found here: 
%   http://www.activewireinc.com/download.htm
%
% MacOS-X, Linux: ----------------------------------------------------------
%
% ActiveWire is not supported on these systems. There isn't any driver
% for MacOS-X as of 24.10.2006. A Linux port would be possible if some
% volunteer would step in to do that.

% History:
%
% The ActiveWire subfolder is taken from old WinPTB 2.54.
%
% 24.10.2006 Included from PTB 2.54. Modified to be M$-Windows only. (MK)
 
if ~IsWin
   error(['Sorry, ActiveWire is not supported under ' OSName '.']);
else
   error('Could not load ActiveWire.dll - Some Matlab path problem?!?');
end
