function [cal,cals] = LoadCalFile(filespec,whichCal,dir)
% [cal,cals] = LoadCalFile([filespec],[whichCal],[dir])
%
% Load calibration data from saved file in the CalData folder.
% If no argument is given, loads from file default.mat.  If
% an integer N is passed, loads from file screenN.mat.  If
% a string S is given, loads from S.mat.
%
% If whichCal is specified, the whichCal'th calibration
% in the file is returned.  If whichCal > nCals, an
% empty calibration is returned.  whichCal defaults
% to the most recent calibration.
%
% If the specified file cannot be found, returns empty matrix.
%
% The returned variable is a structure containing calibration 
% information.

% 5/28/96  dgp  Wrote it.
% 6/6/96   dgp  Use CalibrationsFolder.
% 6/6/96   dgp  Use whole path in filename so Matlab will only look there.
% 7/25/96  dgp  Use CalDataFolder.
% 8/4/96   dhb  More flexible filename interface.
% 8/21/97	 dhb  Rewrite for calibrations stored as cell array.
%								Optional return of entire calibration history.
% 8/26/97  dhb  Handle case of isempty(cals).
%								Added whichCal argument.
% 5/18/99  dhb  Added dir argument.
% 8/15/00  dhb  Modify to handle local/demo cal directories.

% Get whichCal
if (nargin < 2 | isempty(whichCal))
	whichCal = Inf;
end

% Set the filename
if (nargin < 3 | isempty(dir))
	useDir = CalDataFolder;
else
	useDir = dir;
end
if (nargin < 1 | isempty(filespec))
	filename = [useDir 'default.mat'];
elseif (isstr(filespec))
	filename = [useDir filespec '.mat'];
else
	filename = [useDir sprintf('screen%d.mat',filespec)];
end

% Make sure file is present before calling load
file=fopen(filename);

% If not, make sure to try secondary directory.
if (file == -1 & (nargin < 3 | isempty(dir)))
	useDir = CalDataFolder(1);
	if (nargin < 1 | isempty(filespec))
		filename = [useDir 'default.mat'];
	elseif (isstr(filespec))
		filename = [useDir filespec '.mat'];
	else
		filename = [useDir sprintf('screen%d.mat',filespec)];
	end
	file = fopen(filename);
end

% Now read the sucker if it is there.
if (file ~= -1)
	fclose(file);
	eval(['load ' QuoteString(filename)]);
	if (isempty(cals))
		cal = [];
	else
		nCals = size(cals,2);
		if (whichCal == Inf)
			cal = cals{nCals};
		elseif (whichCal > nCals)
			cal = [];
		else
			cal = cals(whichCal);
		end
	end
else
	cal = [];
	cals = {};
end
