function SaveCalFile(cal,filespec,dir)
% SaveCalFile(cal,[filespec],[dir])
%
% Saves calibration data in the structure "cal" to a
% calibration file in the CalData folder.
%
% If filespec is not passed, then it saves to default.mat
% in the CalData folder.  If filespec is an integer, saves
% to screenN.mat.  If filespec is a string, saves to string.mat.

% 5/28/96  dgp  Wrote it.
% 6/6/96   dgp  Use CalibrationsFolder.
% 7/25/96  dgp  Use CalDataFolder.
% 8/4/96   dhb  More flexible filename interface.
% 8/21/97	 dhb  Rewrite for cell array convention.
% 8/25/97  dhb, pbe  Fix bug in cell array handling.
% 8/26/97  dhb  Make saving code parallel LoadCalFile.
% 5/18/99  dhb  Add optional directory arg.
% 8/10/00  dhb  Fix loading code for default.mat
% 7/9/02   dhb  Incorporate filespec/filename fix as suggested by Eiji Kimura.

% Set the filename
if (nargin < 3 | isempty(dir))
	dir = CalDataFolder;
end
if (nargin < 2 | isempty(filespec))
	filespec = 'default';
	filename = [dir 'default.mat'];
elseif (isstr(filespec))
	filename = [dir filespec '.mat'];
else
	filename = [dir sprintf('screen%d.mat',filespec)];
end

% Load the file to get older calibrations
[oldCal,oldCals] = LoadCalFile(filespec);
if (isempty(oldCals))
	cals = {cal};
else
	nOldCals = size(oldCals,2);
	cals = oldCals;
	cals{nOldCals+1} = cal;
end

% Save the file
eval(['save ' QuoteString(filename) ' cals']);;
	
