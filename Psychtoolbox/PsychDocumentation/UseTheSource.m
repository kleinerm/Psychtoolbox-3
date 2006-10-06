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
% 3. The following command would download the full PTB distribution
%    into the subdirectory /Users/kleinerm/MyPTB :
%    svn checkout svn://svn.berlios.de/osxptb/trunk /Users/kleinerm/MyPTB/
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
%    trunk  - The code that is currently under development.
%    beta   - Source code corresponding to the 'beta' release, derived
%             from 'trunk'.
%    stable - Source code corresponding to the 'stable' release, derived
%             from 'beta'.
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
%    Psychtoolbox is free software; you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation; either version 2 of the License, or
%    (at your option) any later version. The file License.txt in the
%    Psychtoolbox root folder contains the exact conditions for use,
%    modification and redistribution.  
%
%    Good luck and "Use the source" (Master Yoda).
