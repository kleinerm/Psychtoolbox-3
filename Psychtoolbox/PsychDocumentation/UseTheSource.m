% UseTheSource - How to access the Psychtoolbox source code.
%
% If you are interested in the source code for the C language
% implementation of Psychtoolbox (i.e., the MEX file extensions
% like Screen, WaitSecs, GetSecs, PsychHID, ...) then use the
% following procedure.
%
% The Psychtoolbox source code is stored in a Subversion code
% management system. To download the source code:
%
% 1. Open a Unix terminal window on OS-X or Linux or a NT command
%    shell on M$-Windows.
%
% 2. Use the "svn" command line tool to download source code. On
%    OS-X or Linux, the svn tool is usually stored in the
%    /usr/local/bin directory, you may want to add this to your
%    PATH. On Windows, the svn client should be already in your
%    %PATH%
%
% 3. The following command would download the current development branch of
%    the full PTB distribution into the subdirectory /Users/kleinerm/MyPTB :
%
%    svn checkout https://svn.berlios.de/svnroot/repos/osxptb/trunk /Users/kleinerm/MyPTB/
%
%    This command would download the full 'beta' distribution,
%    corresponding to official, tested, production releases:
%
%    svn checkout https://svn.berlios.de/svnroot/repos/osxptb/beta /Users/kleinerm/MyPTB/
%
%    Inside the /Users/kleinerm/MyPTB you would see the following
%    subfolders:
%    Psychtoolbox/   -- The regular Psychtoolbox folder.
%    managementools/ -- Miscellaneous scripts for managing PTB releases
%    PsychSourceGL/  -- The C source code for Psychtoolbox.
%
%    Inside PsychSourceGL you will find everything you need to
%    understand or modify PTBs working if you are brave and patient
%    enough ;-)
%
%    The toolbox itself is split into multiple branches:
%
%    trunk  - The code that is currently under development. May or may not
%             compile and work at any given point in time, may or may not
%             be consistent. For developers and early testers only!
%
%    beta   - Source code corresponding to the current 'beta' release, derived
%             from 'trunk'. This is what normal users should use, because
%             it is somewhat tested.
%
%    If you type the command "PsychtoolboxVersion" in Matlab or Octave, it
%    will tell you the flavor (trunk or beta) and SVN revision number of
%    your currently installed PTB distribution. This allows you to locate
%    the corresponding code in the SVN repository.
%
%    Be aware that you can only download code from the repository. If
%    you want to participate in PTBs development you'll need write access
%    to the repository. Contact David Brainard if you want to apply for
%    write access. Make yourself familiar with the Subversion toolchain
%    if you want to do serious work (http://subversion.tigris.org).
%
%    You can also browse the source code and all development logs via a
%    web interface: http://svn.berlios.de/wsvn/osxptb
%
%    For almost all source code, i.e., unless otherwise stated in the
%    source files or accompanying documentation themselves, the
%    MIT license applies. See the file License.txt in the Psychtoolbox
%    root folder for more details.
%
%    Components & Libraries which are not covered by the MIT license, but
%    other MIT compatible free software licenses will carry individual
%    descriptions of their licenses inside the source files or in
%    accompanying license files within the corresponding subfolders. Such
%    libraries are usually contained in the PsychSourceGL/Cohorts/
%    subdirectory of the distribution.
%
%    Good luck and "Use the source" (Master Yoda).
%
