function CMClose(meterType)
% CMClose([meterType])
%
% Close color meter interface.
%
% meterType 1 is the PR650 (default)
% meterType 2 is the CVI (need CVIToolbox)
% meterType 3 is the CRS Colorimeter
%
% 2/15/02  dhb  Wrote it.
% 4/13/02  dgp	Cosmetic.
% 2/26/03	 dhb  Added more meter types. 
% 3/27/03  dhb, jmh  Fix up default argument.

% Set default meterType.
if nargin < 1 || isempty(meterType)
  meterType = 1;
end

switch meterType
	case 1
		% PR-650
		if IsWin || IsOS9 || IsOSX
			PR650close;
		else
			error(['Unsupported OS ' computer]);
		end
	case 2
		if ~exist('CVIClose')
			error('Need CVIToolbox to call CVIClose (meterType 2)');
		end
		CVIClose;
	case 3
		CRSColorClose;
	otherwise,
		error('Unknown meter type');
end

