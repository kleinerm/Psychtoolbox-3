function CMCheckInit(meterType)
% CMCheckInit([meterType])
%
% Initialize the color meter.  The routine calls the
% lower level CMETER.  If the low level routine
% fails, the one prompts the user to take appropriate
% action.  If the low level routine succeeds, this
% routine  is silent.
%
% meterType 1 is the PR650 (default)
% meterType 3 is the CRS Colorimeter
%
% 9/15/93 dhb		Modified to use new CMInit properly.
% 1/18/94 jms		Added gHardware switch
% 1/24/94 dhb		Changed sign of gHardware switch.
% 2/20/94 dhb		Call through CMETER rather than CM... routines.
% 4/4/00  dhb       Optional port name, only used on SERIAL version.
% 9/11/00 dhb       Added meterType.
% 1/4/02  dhb, ly   Try to get OS9 version to work with Megawolf board and SERIAL.
% 1/10/02 dhb, ly   Change calling convention.  Remove passing of port, but read
%                   port from a "calibration" file in PsychCalLocalData if it's there.
% 4/13/02 dgp		Cosmetic.
% 2/26/03 dhb   Add CRS Colorimeter, change meter type of PR-650 to 1.
% 10/05/06 dhb, cgb OSX version.

% Set default meterType.
if nargin<1 | isempty(meterType)
  meterType = 1;
end

switch meterType
	case 1,
		% PR-650
		% Look for port information in "calibration" file.  If
		% no special information present, then use defaults.
		meterports = LoadCalFile('PR650Ports');
        if isempty(meterports)
            if IsWin
                portNameIn = 'COM5';
            elseif IsOSX
                portNameIn = 2;
            else
                error(['Unsupported OS ' computer]);
            end

        else
            portNameIn = meterports.in;
        end

		if IsWin || IsOSX
			% Interface through PsychSerial for both OS9 and Windows.
			stat = PR650init(portNameIn);				
			status = sscanf(stat,'%f');
			if (isempty(status) || status == -1)
			  disp('If colorimeter is off, turn it on');
			end
			while isempty(status) || status == -1
				stat = PR650init(portNameIn);
				status = sscanf(stat,'%f');
			end
		else
			error(['Unsupported OS ' computer]);
		end
	case 3,
		CRSColorInit;
	otherwise,
		error('Unknown meter type');
end

