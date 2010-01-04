% Psychtoolbox:PsychContributed
%
% Scripts and software contributed by Psychtoolbox users and
% third-party tools and software that are used in Psychtoolbox. These
% third-party tools are covered by different licenses which allow distribution
% with Psychtoolbox, but are not neccessarily free-software or open-source.
% See the descriptions below or the help files of the functions for
% exact licensing conditions of individual contributions.
%
% help Psychtoolbox % For an overview, triple-click me & hit enter.
%
%
% ATIRadeonperf                ATIRadeonperf_Linux and ATIRadeonperf_Windows.exe are
%                              compiled Linux and Windows executables that are called
%                              as helpers from the function PsychGPUControl() via a
%                              system() call, ie., they execute in the system shell as
%                              separate processes.
%
%                              These binaries perform the actual GPU setup work on behalf
%                              of PsychGPUControl() using ATI/AMD proprietary low-level
%                              API's. The API's are available to registered developers from
%                              AMD/ATI as "ADL SDK" without royalty fees. While the SDK
%                              license does allow redistribution of executables using the
%                              SDK, it doesn't allow redistribution in source code form.
%                              For that reason, these tools are only made available as
%                              precompiled binaries for your use, not under any open-source
%                              or free-software license. The tools were compiled by Mario
%                              Kleiner, so contact him if you have further questions about
%                              the tools.
%
% simplepsychtoolboxsetup.sh   Configure OS X priority.  Call from Unix shell, not MATLAB.
%
% macid.exe                    Microsoft Windows command line utility for retrieving the MAC
%                              ethernet address of the primary network
%                              adapter. This free software GPL utility is
%                              downloaded from www.mirkes.de Windows batch
%                              tools. Author: Markus Stephany
%                              URL:
%                              http://www.mirkes.de/en/freeware/batch.php
%
% macidpascalsource/           This folder contains the Delphi-7 Pascal source code
%                              of macid.exe, as required by macid's GPL license.
%
% ple                          Prints useful debugging output, including a stack trace in case
%                              a script exits via rethrow() or psychrethrow(). See help ple for
%                              copyright info.
%
% read_rle_rgbe                Reader routine for RLE encoded RGBE high dynamic range images.
%                              See help read_rle_rgbe for copyright & authorship info. This is
%                              a helper routine for ReadHDR - the recommended function for user
%                              scripts.
%
% WinJoystickMex               WinJoystickMex is a very simple/rudimentary
%                              joystick driver for basic query of position
%                              and button state of joysticks under
%                              MS-Windows, written by Mario Kleiner. The
%                              code is contributed under GPLv2.
%
% WinTab/                      This folder contains the (unsupported!)
%                              WinTabMex MEX file driver (and its C source
%                              code) for controlling touch/digitizer
%                              tablets via the WinTab API on Microsoft
%                              Windows operating systems. It also contains
%                              some basic usage instructions and two M-File
%                              demos, contributed by Andrew Wilson.
%
