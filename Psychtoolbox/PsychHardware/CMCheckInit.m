function CMCheckInit(meterType, PortString)
% CMCheckInit([meterType], [PortString])
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
% 11/27/07  mpr replaced hard coded portNameIn with FindSerialPort for OS X, and
%                 attempted to make this more robust and user-friendly.
%                 modifications tested only on Mac OS 10.5.1, Matlab 2007b, on
%                 a Mac Pro.  Other systems may require more tinkering...

DefaultNumberOfTries = 5;

% Set default the defaults.
switch nargin
	case 0
		meterType = 1;
		PortString = 'usbserial';
	case 1
		if isempty(meterType)
			meterType = 1;
		end
		PortString = 'usbserial';
	case 2
		if isempty(meterType)
			meterType = 1;
		end
		if isempty(PortString)
			PortString = 'usbserial';
		end
end

% I wrote the function FindSerialPort before I discovered CMCheckInit had
% been ported to OS X.  It may generally require less of users than relying
% on what amounts to a preference in the calibration files.  The default 
% for portNameIn was 2, but on my machine it was 1 when I wrote my
% function.  Someone in the Brainard lab may want to generalize
% "FindSerialPort" to work with OSs other than Mac OS X and then use that
% function in lieu of meterports=LoadCalFile.  I am not intrepid enough to
% take that step.  -- MPR 11/21/07


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
                portNameIn = FindSerialPort(PortString);
            else
                error(['Unsupported OS ' computer]);
            end

        else
            portNameIn = meterports.in;
        end

		if IsWin || IsOSX
			% Interface through PsychSerial for both OS X and Windows.
			stat = PR650init(portNameIn);				
			status = sscanf(stat,'%f');
			if (isempty(status) || status == -1)
        disp('Failed initial attempt to make contact.');
			  disp('If colorimeter is off, turn it on; if it is on, turn it off and then on.');
      end
      NumTries = 0;
  
			while (isempty(status) || status == -1) & NumTries < DefaultNumberOfTries 
				stat = PR650init(portNameIn);
				status = sscanf(stat,'%f');
        NumTries = NumTries+1;
        if (isempty(status) || status == -1) & NumTries >= 3
          clear global g_serialPort;
          if IsOSX
            evalc(['SerialComm(''close'',' int2str(portNameIn) ');']);
            evalc('clear SerialComm');
          end
          fprintf('\n');
          if ~rem(NumTries,4)
            fprintf('\nHave tried making contact %d times.  Will try %d more...',NumTries,DefaultNumberOfTries-NumTries);
          end
        end
      end
      fprintf('\n');
      if isempty(status) || status == -1
        disp('Failed to make contact.  If device is connected, try turning it off and re-trying CMCheckInit.');
      else
        disp('Successfully connected to PR-650!');
      end
		else
			error(['Unsupported OS ' computer]);
		end
	case 3,
		CRSColorInit;
	otherwise,
		error('Unknown meter type');
end

