function DownloadLegacyPsychtoolbox
% DownloadLegacyPsychtoolbox
%
% THIS SCRIPT IS DEAD! Contact the PTB forum for help if you need old
% copies of PTB, or better check on the PTB websites "Versions" section for
% download instructions for old copies of Psychtoolbox prior to 3.0.10.
%

% History:
%
% 11/02/05 mk  Created.
% 11/25/05 mk  Bug fix for 'targetdirectory' provided by David Fencsik.
% 01/13/06 mk  Added support for download of Windows OpenGL-PTB.
% 03/10/06 dgp Expanded the help text, above, incorporating suggestions
%              from Daniel Shima.
% 03/11/06 dgp Check OS. Remove old Psychtoolbox from path and from disk.
%              After downloading, add new Psychtoolbox to path.
% 03/12/06 dgp Polished error message regarding missing svn.
% 03/13/06 dgp Changed default targetdirectory from PWD to ~/Documents/.
% 03/14/06 dgp Changed default targetdirectory to /Applications/, and
%              if not sufficiently privileged, then /Users/Shared/.
%              Check privilege to create folder. Check SAVEPATH.
% 06/05/06 mk  On Windows, we require the user to pass the full path to
%              the installation folder, because there is no well-defined
%              equivalent to the Mac OS X /Applications/ folder. Also, the order
%              of operations was changed to maximize the chance of getting a
%              working PTB installation despite minor failures of commands
%              like savepath, or fileattrib. We allow the user to decide
%              whether to delete her old Psychtoolbox folders or to
%              retain multiple copies of Psychtoolbox (e.g., beta and stable)
%              so each user can choose between Beta and Stable.
%              We no longer copy UpdatePsychtoolbox.m, since it's included in the 
%              new Psychtoolbox folder 
% 06/06/06 dgp Cosmetic editing of comments, above.
% 06/27/06 dgp Cosmetic editing of comments and messages. Check for spaces
%              in targetdirectory name.
% 9/23/06  mk  Add clear mex call to flush mex files before downloading.
% 10/5/06  mk  Add detection code for MacOS-X on Intel Macs.
% 10/28/06 dhb Allow 'current' as a synonym for 'beta'.
% 11/21/06 mk  Allow alternate install location for svn client: Installer
%              will find the svn executable if its installation folder is
%              included in the Matlab path.
% 02/17/07 mk  Convert flavor spec to lower case in case it isn't.
% 03/15/07 mk  Detection code for Windows 64 bit added.
% 07/18/07 mk  Changed default for flavor from 'stable' to 'beta'.
% 09/27/07 mk  Add another fallback path: Download via https protocol to
%              maybe bypass proxy-servers.
% 10/29/07 mk  Small fix for Kerstin Preuschoffs bugreport: Download of old
%              versions didn't work anymore, becuase 'flavor' string was
%              lower-cased.
% 11/17/07 mk  Prioritized https:// checkout over http:// checkout --> More
%              likely to bypass proxy servers and succeed.
% 12/25/07 mk  Add check for white spaces in path to subversion client - Output warning, if so.
% 12/25/07 mk  Add optional 'downloadmethod' parameter, which allows to
%              select initial choie of download protocol to use, in order to allow to
%              bypass misconfigured proxies and firewalls. Problem reported
%              by Patrick Mineault.
% 01/08/08 mk  On OS/X, add an additional search path for 'svn': /usr/bin/
%              as per suggestion of Donald Kalar. Presumably, Apples Leopard ships
%              with a svn client preinstalled in that location.
%
% 05/07/08 mk  Add better handling of default values. Add 'targetRevision'
%              parameter as option, just as in UpdatePsychtoolbox.m
%
% 06/16/08 mk  Change default initial download protcol from svn: to https:,
%              as Berlios seems to have disabled svnserve protocol :-(
%
% 10/01/08 mk  Add interactive output/query for svn client on the Unices.
%              This to work-around questions of the client about accepting
%              security certificates...
% 01/05/09 mk  Remove && or || to make old Matlab versions happier.
% 01/05/09 mk  Remove && or || to make old Matlab versions happier.
% 01/05/09 mk  Change order of call arguments to make choice of default
%              'beta' flavor more convenient and choice of 'stable'
%              inconvenient. Also add some additional warning text and
%              dialog to tell user how bad 'stable' is and to give him a
%              chance to reconsider.
% 03/22/09 mk  Update help text again. Rename 'stable' into 'unsupported'.
% 05/31/09 mk  Add support for Octave-3.
% 10/05/09 mk  Strip trailing fileseperator from targetDirectory, as
%              suggested by Erik Flister to avoid trouble with new svn
%              clients.
% 12/27/10 mk  Redirect 'beta' downloads on Matlab versions < 6.5 to the
%              special "Psychtoolbox-3.0.8-PreMatlab6.5" compatibility
%              version - The last one to support pre 6.5 Matlab's.
%
% 10/31/11 mk  Change location of SVN repository to
%              http://psychtoolbox-3.googlecode.com/svn/
%              our new home, now that Berlios is shutting down.
%
% 05/30/12 mk  Fix disasterous bug: Answering anything but 'y' to the
%              question if old PTB folder should be deleted, caused
%              *deletion* of the folder! Oh dear! This bug present since
%              late 2011.
% 11/16/16 mk  Killed it.

clc;
help DownloadLegacyPsychtoolbox;
return;
