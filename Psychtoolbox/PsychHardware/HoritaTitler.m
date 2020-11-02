function HoritaTitler(command,varargin)
% HoritaTitler(command [, arg1, arg2, ...])
%
% This function establishes a serial port connection to the HORITA SCT-50 video
% titler and compatible devices, and performs a number of commands. Commands are
% based on the HORITA SCT-50 video titler user manual (https://horita.com/).
%
%
% Commands:
%
%   HoritaTitler('Open' [, 'serialPort'][, 'HORITAaddress'])
%
%       THIS FUNCTION SHOULD BE CALLED FIRST!!
%
%       Initializes and opens the serial port which establishes communication to the HORITA.
%       If serialPort/HORITAaddress is ommitted or left empty, the defaultPort is 'COM4' on
%       MS-Windows, '/dev/tty.KeySerial1' on Apple macOS, and '/dev/ttyS0' on GNU/Linux.
%       The Psychtoolbox function FindSerialPort() can be helpful to automatically find a
%       connected serial port device.
%
%       The default HORITAaddress is '02'.
%
%   HoritaTitler('Close')
%
%       Closes the serial connection with HORITA.
%
%   HoritaTitler('SetAddress', 'HORITAaddress')
%
%       Set the 'HORITAaddress' for the target HORITA titler device. Followup commands will
%       be sent to the new address. This allows to select between multiple daisy-chained
%       HORITA titler devices. Valid HORITAADDRESSes are between '00' and '99', with a default
%       address of '02'. The special address '00' broadcasts to / selects all connected HORITA
%       devices, so they all execute the same command simultaneously.
%
%   HoritaTitler('Write' [, 'text', row, column])
%
%       Writes text at a specified position on the screen. Positions are numbered from 001 (top
%       left) to 180 (bottom right), and there are a total number of 9 rows and 20 columns
%       available. Text is automatically converted to uppercase. If row/colum are omitted or left
%       empty, the default is to write on row/column 1. If text is omitted, a blank space is
%       inserted at the specified position. If the text does not fit on one row, then it is
%       shortened.
%
%   HoritaTitler('Write' [, number, row, column])
%
%       Writes numbers at a specified position on the screen. Numbers are converted from double
%       class to string class using mat2str(number). If row/column are omitted or left empty, the
%       default is to write on row/column 1. If number is omitted, a blank space is inserted at the
%       specified porition.
%
%   HoritaTitler('WriteCont' [, 'text'])
%
%       Writes text at the current position of the cursor. If no text is provided, a blank space is
%       inserted at the current position.
%
%   HoritaTitler('WriteCont' [, number])
%
%       Writes numbers at the current position of the cursor. Numbers are converted from double
%       class to string class using mat2str(number). If no number is provided, a blank space is
%       inserted at the current position.
%
%   HoritaTitler('Clear')
%
%       Clears the display (requires 100 ms).
%
%   HoritaTitler('ClearLine'[, row])
%
%       Clears the given row on the display. If the row number is omitted, clears the first line.
%       HORITA display has 9 rows, therefore row <= 9.
%
%   HoritaTitler('Reset')
%
%       Resets the HORITA titler (requires 300 ms). All variable data in the HORITA SCT-50 are set
%       to default values.
%
%   HoritaTitler('TimeOn')
%
%       Turns the time display on.
%
%   HoritaTitler('TimeOff')
%
%       Turns the time display off.
%
%   HoritaTitler('TimeSet' [, 'hour', 'minute', 'second'])
%
%       Sets the time. Hour/minute/second have to be 2 digits long. If hour/minute/second are
%       omitted or empty, the time is set based on the Matlab clock function.
%
%   HoritaTitler('TimePosit' [, row, column, 'hour', 'minute', 'second'])
%
%       SET THE TIME ON BEFORE!
%
%       Sets the time and places it at the specified position. Time display has to be set on prior
%       to the positioning. If row/column are omitted or left empty, the time is placed on row/column 1.
%       Hour/minute/second have to be 2 digits long. If hour/minute/second are omitted or empty, the
%       time is set based on the Matlab clock function.
%
%   HoritaTitler('TimeFormat' [, 'format'])
%
%       Sets the time display format. There are 36 time display formats. The default time format is
%       'hh:mm:ss AM' (i.e. format '05'). Format '02' will display hh:mm:ss. Format '03' will
%       display hh:mm:ss.ms.
%
%   HoritaTitler('DateOn')
%
%       Turns the date display on.
%
%   HoritaTitler('DateOff')
%
%       Turns the date display off.
%
%   HoritaTitler('DateSet' [, 'day', 'month', 'year'])
%
%       Sets the date. Month/day/year have to be 2 digits long. If month/day/year are omitted or
%       empty, the time is set based on the Matlab clock function.
%
%   HoritaTitler('DatePosit' [, row, column, 'day', 'month', 'year'])
%
%       SET THE DATE ON BEFORE!
%
%       Sets the date and places it at the specified position. Date display has to be set on prior
%       to the positioning. If row/column are omitted or left empty, the date is placed on
%       row/column 1. Month/day/year have to be 2 digits long. If month/day/year are omitted or
%       empty, the date is set based on the Matlab clock function.
%
%   HoritaTitler('DateFormat' [, 'format'])
%
%       Sets the date display format. There are 60 display formats. The default date format is
%       'dd/mo/yy' (i.e. format '10', 23/06/17). Format '11' will display 23/JUN/17. Format '59'
%       will display 23JUN17.
%
%   HoritaTitler('BackgroundOn')
%
%       Sets on the text background.
%
%   HoritaTitler('BackgroundOff')
%
%       Sets off the text background.
%
%   HoritaTitler('TextWhite')
%
%       Sets the text to white.
%
%   HoritaTitler('TextWhite')
%
%       Sets the text to black.
%
%   HoritaTitler('DeleteSerial')
%
%       Close all serial ports. Good to do this right at the beginning, before
%       initializing HORITA, in case a previous serial connection is still open.
%
%
% ===================================================================================
% Created by Natasa Ganea, 2017, Goldsmiths Infantlab (email: natasa.ganea@gmail.com)
% Licensed to you under the MIT open-source license.
% ===================================================================================

% History:
%
% ??-???-2017   Natasa Ganea    Written.
% 09-May-2020   mk              Ported from serial() to IOPort(), minor cross-platform
%                               enhancements, new 'SetAddress' subfunction.
%

    persistent HORITAADDRESS;       % HORITA address; found in HORITA SETTINGS
    persistent DEVICE;              % serial port object in Matlab; HORITA receives serial input from PC
    persistent HORITAMATRIX;        % HORITA display matrix (9 rows, 20 columns)

    if nargin < 1 || isempty(command)
        help HoritaTitler;
        return;
    end

    %% %% HORITA Commands 

    switch lower(command)

        case 'open'
            % Initializes the serial port object in Matlab and the HORITA positions matrix

            % Default serial port and HORITA address
            defaultAddress = '02';                   % HORITA address found in HORITA Settings

            if IsWin
                defaultPort = 'COM4';                % serial port on Windows
            end

            if IsOSX
                defaultPort = '/dev/tty.KeySerial1'; % serial port on Mac
            end

            if IsLinux
                defaultPort = '/dev/ttyS0';          % serial port on Linux
            end

            % if serial port is omitted or empty, use default
            if nargin < 2 || isempty(varargin{1})
                serialPort = defaultPort;
            else
                serialPort = varargin{1};
            end

            DEVICE = IOPort('OpenSerialPort', serialPort, 'Lenient BaudRate=9600 DataBits=8');

            % if HORITA address is omitted or empty, use default
            if nargin < 3 || isempty(varargin{2})
                HORITAADDRESS = defaultAddress;
            else
                HORITAADDRESS = varargin{2};
            end

            % initialize HORITA positions matrix (9 rows, 20 columns)
            HORITAMATRIX(1,:) = 1:20;
            HORITAMATRIX(2,:) = 1 .* 20 + HORITAMATRIX(1, :);
            HORITAMATRIX(3,:) = 2 .* 20 + HORITAMATRIX(1, :);
            HORITAMATRIX(4,:) = 3 .* 20 + HORITAMATRIX(1, :);
            HORITAMATRIX(5,:) = 4 .* 20 + HORITAMATRIX(1, :);
            HORITAMATRIX(6,:) = 5 .* 20 + HORITAMATRIX(1, :);
            HORITAMATRIX(7,:) = 6 .* 20 + HORITAMATRIX(1, :);
            HORITAMATRIX(8,:) = 7 .* 20 + HORITAMATRIX(1, :);
            HORITAMATRIX(9,:) = 8 .* 20 + HORITAMATRIX(1, :);

        case 'close'
            % Closes the HORITA titler
            if ~isempty(DEVICE)
                IOPort('Close', DEVICE);
            end

            DEVICE = [];

        case 'setaddress'
            % Assign new HORITAADDRESS
            HORITAADDRESS = varargin{1};

        case 'write'
            % Writes text/digits at a given position To send command to HORITA we have to start
            % command with 27 = ASCII escape, and follow 27 with HORITAADDRESS (char) and HORITAcmd
            % (char).
            HORITAcmd = '28';

            % if the HORITA text is omitted or empty, leave blank space
            if nargin < 2 || isempty(varargin{1})
                HORITAtext = ' ';
            elseif isa(varargin{1}, 'double')
                HORITAtext = mat2str(varargin{1}); % if input is number, convert to string
            else
                HORITAtext = char(varargin{1});
                HORITAtext = upper(HORITAtext); % HORITA likes capital letters :)
            end

            % if HORITA row is omitted or empty, row = 1
            if nargin < 3 || isempty(varargin{2})
                HORITArow = 1;
            else
                HORITArow = varargin{2};
            end

            % if HORITA column is omitted or empty, column = 1
            if nargin < 4 || isempty(varargin{3})
                HORITAcolumn = 1;
            else
                HORITAcolumn = varargin{3};
            end

            if (HORITArow >= 1) && (HORITArow <= 9) && (HORITAcolumn >= 1) && (HORITAcolumn <= 19)...
                    && ~isempty(HORITAtext)

                % shorten text if it is longer than HORITA row
                if length(HORITAtext) > (20-HORITAcolumn)
                    HORITAtext = HORITAtext(1: 20-HORITAcolumn);
                end

                % get HORITA cursor position and set it as 'string'
                HORITAposition = mat2str(HORITAMATRIX(HORITArow,HORITAcolumn));
                if length(HORITAposition) < 2               % if position is < 10 put 2 zeros before
                    HORITAposition = ['00' HORITAposition];
                elseif length(HORITAposition) < 3
                    HORITAposition = ['0' HORITAposition];  % if position is < 100 put 1 zero before
                end

                % send text to HORITA at the specified location
                IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd HORITAposition HORITAtext]));

            else
                disp('Check: row (1st number) & column (2nd number)!')
                disp('Row should be < 9. HORITA has 9 rows.')
                disp('Column should be < 19. HORITA has 20 columns.')
            end

        case 'writecont'
            % Continue writing
            HORITAcmd = '27';

            % if HORITA text is omitted, leave blank space
            if nargin < 2
                HORITAtext = ' ';
            elseif isa(varargin{1}, 'double')
                HORITAtext = mat2str(varargin{1}); % if input is number, convert to string
            else
                HORITAtext = char(varargin{1});
                HORITAtext = upper(HORITAtext);    % HORITA likes capital letters :)
            end

            % send text to HORITA (at the current position of the HORITA cursor)
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd HORITAtext]));

        case 'clear'
            % Clears text from screen and HORITA memory
            HORITAcmd = '25';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));
            % HORITA manual: HORITA requires 100ms to clear all lines
            pause(.1);

        case 'clearline'
            % Clears text from specified row
            HORITAcmd = '24';

            % if HORITA row is omitted, clear row 1
            if nargin> 1
                HORITArow = varargin{1};
            else
                HORITArow = '1';
            end

            % display error if row number is too big
            if (HORITArow >= 1) && (HORITArow <= 9)
                HORITArow = mat2str(HORITArow);
                IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd HORITArow]));
            else
                disp('Check row (row should be < 9)!')
            end

        case 'reset'
            % Resets the HORITA titler
            HORITAcmd = '00';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));
            % HORITA manual: HORITA requires 300ms to reset
            pause(.3)

        case 'timeon'
            % Turns on time display
            HORITAcmd = '34';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));

        case 'timeoff'
            % Turns off time display
            HORITAcmd = '35';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));

        case 'timeset'
            % Sets the time
            HORITAcmd = '36';

            % check Matlab clock
            format shortg
            clockMatlab = fix(clock);

            % if hour is omitted or empty, get Matlab hour
            if nargin < 2 || isempty(varargin{1})
                hh = clockMatlab(1,4);
                hh = mat2str(hh);      % HORITA likes char/str
            else
                hh = varargin{1};
            end

            % if minute is omitted or empty, get Matlab minute
            if nargin < 3 || isempty(varargin{2})
                mm = clockMatlab(1,5);
                mm = mat2str(mm);
            else
                mm = varargin{2};
            end

            % if second is omitted or empty, get Matlab second
            if nargin < 4 || isempty(varargin{3})
                ss = clockMatlab(1,6);
                ss = mat2str(ss);
            else
                ss = varargin{3};
            end

            % hour/minute/second have to be 2 digits long
            timeIn = {hh, mm, ss};
            timeInCharNum = [length(hh), length(mm), length(ss)]; % check number of characters
            for i = 1:length(timeInCharNum)
                if timeInCharNum(i) < 2                           % if number of characters < 2
                    timeIn(i) = strcat('0', timeIn(i));           % add a zero before hh/mm/ss
                end
            end
            HORITAtime = strcat(timeIn(1), timeIn(2), timeIn(3)); % time string (is 11 char long)
            HORITAtime = char(HORITAtime); %HORITA likes char input

            % send time to HORITA
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd HORITAtime]));

        case 'timeposit'
            % Sets the time and places it at a given location
            HORITAcmd = '37';

            % if HORITA row is omitted or empty, row = 1
            if nargin < 2 || isempty(varargin{1})
                HORITArow = 1;
            else
                HORITArow = varargin{1};
            end

            % if HORITA column is omitted or empty, column = 1
            if nargin < 3 || isempty(varargin{2})
                HORITAcolumn = 1;
            else
                HORITAcolumn = varargin{2};
            end

            % position the time on the display (max time format = 11 char long)
            if (HORITArow >= 1) && (HORITArow <= 9) && (HORITAcolumn >= 1) && (HORITAcolumn <= 9)

                % get HORITA cursor position and set it as 'string'
                HORITAposition = mat2str(HORITAMATRIX(HORITArow,HORITAcolumn));
                if length(HORITAposition) < 2              % if position is < 10 put 2 zeros before
                    HORITAposition = ['00' HORITAposition];
                elseif length(HORITAposition) < 3          % if position is < 100 put 1 zero before
                    HORITAposition = ['0' HORITAposition];
                end

                % setting the HORITA time (required)
                % check Matlab clock
                format shortg
                clockMatlab = fix(clock);

                % if hour is omitted or empty, get Matlab hour
                if nargin < 4 || isempty(varargin{3})
                    hh = clockMatlab(1,4);
                    hh = mat2str(hh);      % HORITA likes char/str
                else
                    hh = varargin{3};
                end

                % if minute is omitted or empty, get Matlab minute
                if nargin < 5 || isempty(varargin{4})
                    mm = clockMatlab(1,5);
                    mm = mat2str(mm);
                else
                    mm = varargin{4};
                end

                % if second is omitted or empty, get Matlab second
                if nargin < 6 || isempty(varargin{5})
                    ss = clockMatlab(1,6);
                    ss = mat2str(ss);
                else
                    ss = varargin{5};
                end

                % hour/minute/second have to be 2 digits long
                timeIn = {hh, mm, ss};
                timeInCharNum = [length(hh), length(mm), length(ss)]; % check number of characters
                for i = 1:length(timeInCharNum)
                    if timeInCharNum(i) < 2                           % if number of characters < 2
                        timeIn(i) = strcat('0', timeIn(i));           % add a zero before hh/mm/ss
                    end
                end
                HORITAtime = strcat(timeIn(1), timeIn(2), timeIn(3)); % time string (is 11 char long)
                HORITAtime = char(HORITAtime); %HORITA likes char input

                % send time HORITA specifying time position
                IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd HORITAposition HORITAtime]));

            else
                disp('Check: row (1st number) & column (2nd number)!')
                disp('Row should be < 9. HORITA has 9 rows.')
                disp('Column should be < 9. HORITA time is 11 characters long.')
            end

        case 'timeformat'
            % Sets the time display format
            HORITAcmd = '48';

            % if time display is omitted or empty, set format to '05'
            if nargin < 2 || isempty(varargin{1})
                timeFormat = '05'; %hh:mm:ss AM
            else
                timeFormat = varargin{1};
            end

            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd timeFormat]));

        case 'dateon'
            % Turns on the date display
            HORITAcmd = '39';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));

        case 'dateoff'
            % Turns off the date display
            HORITAcmd = '40';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));

        case 'dateset'
            % Sets the HORITA date
            HORITAcmd = '41';

            % check Matlab clock
            format shortg
            clockMatlab = fix(clock);

            % if day is omitted or empty, get Matlab day
            if nargin < 2 || isempty(varargin{1})
                dd = clockMatlab(1,3);
                dd = mat2str(dd);
            else
                dd = varargin{1};
            end

            % if month is omitted or empty, get Matlab month
            if nargin < 3 || isempty(varargin{2})
                mo = clockMatlab(1,2);
                mo = mat2str(mo);      % HORITA likes char/str
            else
                mo = varargin{2};
            end

            % if year is omitted or empty, get Matlab year
            if nargin < 4 || isempty(varargin{3})
                yy = clockMatlab(1,1);
                yy = mat2str(yy);
            else
                yy = varargin{3};
            end

            % month/day/year have to be 2 digits long
            dateIn = {mo, dd, yy};   %HORITA like date input: mmddyy (month first) :)
            dateInCharNum = [length(mo), length(dd), length(yy)]; % check number of characters
            for i = 1:length(dateInCharNum)
                if dateInCharNum(i) < 2                           % if number of characters < 2
                    dateIn(i) = strcat('0', dateIn(i));           % add a zero before mo/dd
                elseif dateInCharNum(i) > 2
                    yy = [yy(3) yy(4)]; % HORITA likes 2 char for yy (e.g. '17' instead of '2017')
                    dateIn(3) = {yy};
                end
            end
            HORITAdate = strcat(dateIn(1), dateIn(2), dateIn(3)); % date string
            HORITAdate = char(HORITAdate); % HORITA likes char input :)

            % send date to HORITA
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd HORITAdate]));

        case 'dateposit'
            % Sets the HORITA date and places it at a specified location
            HORITAcmd = '42';

            % if HORITA row is omitted or empty, row = 1
            if nargin < 2 || isempty(varargin{1})
                HORITArow = 1;
            else
                HORITArow = varargin{1};
            end

            % if HORITA column is omitted or empty, column = 1
            if nargin < 3 || isempty(varargin{2})
                HORITAcolumn = 1;
            else
                HORITAcolumn = varargin{2};
            end

            % position of the date on the display (max date format = 11 char long)
            if (HORITArow >= 1) && (HORITArow <= 9) && (HORITAcolumn >= 1) && (HORITAcolumn <= 9)

                % get HORITA cursor position and set it as 'string'
                HORITAposition = mat2str(HORITAMATRIX(HORITArow,HORITAcolumn));
                if length(HORITAposition) < 2              % if position is < 10, put 2 zeros before
                    HORITAposition = ['00' HORITAposition];
                elseif length(HORITAposition) < 3          % if position is < 100, put 1 zero before
                    HORITAposition = ['0' HORITAposition];
                end

                % setting the HORITA date (required)
                % check Matlab clock
                format shortg
                clockMatlab = fix(clock);

                % if day is omitted or empty, get Matlab day
                if nargin < 4 || isempty(varargin{3})
                    dd = clockMatlab(1,3);
                    dd = mat2str(dd);
                else
                    dd = varargin{3};
                end

                % if month is omitted or empty, get Matlab month
                if nargin < 5 || isempty(varargin{4})
                    mo = clockMatlab(1,2);
                    mo = mat2str(mo);      % HORITA likes char/str
                else
                    mo = varargin{4};
                end

                % if year is omitted or empty, get Matlab year
                if nargin < 6 || isempty(varargin{5})
                    yy = clockMatlab(1,1);
                    yy = mat2str(yy);
                else
                    yy = varargin{5};
                end

                % month/day/year have to be 2 digits long
                dateIn = {mo, dd, yy};   %HORITA like date input: mmddyy (month first) :)
                dateInCharNum = [length(mo), length(dd), length(yy)]; % check number of characters
                for i = 1:length(dateInCharNum)
                    if dateInCharNum(i) < 2                           % if number of characters < 2
                        dateIn(i) = strcat('0', dateIn(i));           % add a zero before mo/dd
                    elseif dateInCharNum(i) > 2
                        yy = [yy(3) yy(4)]; % HORITA likes 2 char for yy (e.g. '17' instead of '2017')
                        dateIn(3) = {yy};
                    end
                end
                HORITAdate = strcat(dateIn(1), dateIn(2), dateIn(3)); % date string
                HORITAdate = char(HORITAdate); % HORITA likes char input :)

                % send date to HORITA specifying date position
                IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd HORITAposition HORITAdate]));

            else
                disp('Check: row (1st number) & column (2nd numer)!')
                disp('Row should be < 9. HORITA has 9 rows.')
                disp('Column should be < 9. HORITA date is 11 characters long.')
            end

        case 'dateformat'
            % Sets the date display format
            HORITAcmd = '49';

            % if time display is omitted or empty, set format to '05'
            if nargin < 2 || isempty(varargin{1})
                dateFormat = '10'; %dd/mo/yy
            else
                dateFormat = varargin{1};
            end

            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd dateFormat]));

        case 'backgroundon'
            % Sets text background on
            HORITAcmd = '09';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));

        case 'backgroundoff'
            % Sets text background off
            HORITAcmd = '10';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));

        case 'textwhite'
            % Sets text to white
            HORITAcmd = '11';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));

        case 'textblack'
            % Sets text to black
            HORITAcmd = '12';
            IOPort('Write', DEVICE, uint8([27 HORITAADDRESS HORITAcmd]));

        case 'deleteserial'
            % Removes serial port objects from memory
            IOPort('CloseAll');
            DEVICE = [];

        otherwise
            error (['HoritaTitler: Unknown subcommand ''' command ''' specified.']);

    end

return;
