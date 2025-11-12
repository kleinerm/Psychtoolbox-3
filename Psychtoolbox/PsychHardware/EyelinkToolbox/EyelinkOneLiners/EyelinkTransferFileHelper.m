function EyelinkTransferFileHelper(el, edfFile)
% EyelinkTransferFileHelper(el, edfFile)
%
% Helper function for transferring a copy of the EDF file to the experiment folder
% on the display computer and displaying some feedback to the user.
%
% 'el' Eyelink struct.
% 'edfFile' Name of the EDF file.
%

% History:
% 12-Nov-2025   mk  Written. Based on SR-Research nested function transfer().

try
    % If really connected to EyeLink, not just dummy connected:
    if Eyelink('IsConnected') > 0
        % Show 'Receiving data file...' text until file transfer is complete
        [~, height] = Screen('WindowSize', el.window);
        Screen('FillRect', el.window, el.backgroundcolour); % Prepare background on backbuffer
        Screen('DrawText', el.window, 'Receiving data file...', 5, height-35, 0); % Prepare text
        Screen('Flip', el.window); % Present text
        fprintf('Receiving data file ''%s.edf''\n', edfFile); % Print some text in Matlab's Command Window

        % Transfer EDF file to Host PC
        % Optionally uncomment below to change edf file name when a copy is transferred to the Display PC
        % % If <src> is omitted, tracker will send last opened data file.
        % % If <dest> is omitted, creates local file with source file name.
        % % Else, creates file using <dest> as name.  If <dest_is_path> is supplied and non-zero
        % % uses source file name but adds <dest> as directory path.
        % newName = ['Test_',char(datetime('now','TimeZone','local','Format','y_M_d_HH_mm')),'.edf'];                
        % status = Eyelink('ReceiveFile', [], newName, 0);

        % [status =] Eyelink('ReceiveFile',['src'], ['dest'], ['dest_is_path'])
        status = Eyelink('ReceiveFile');

        % Check if EDF file has been transferred successfully and print file size in Matlab's Command Window
        if status > 0
            fprintf('EDF file size: %.1f KB\n', status/1024); % Divide file size by 1024 to convert bytes to KB
        end
        % Print transferred EDF file path in Matlab's Command Window
        fprintf('Data file ''%s.edf'' can be found in ''%s''\n', edfFile, pwd);
    else
        fprintf('No EDF file saved in Dummy mode\n');
    end
    EyelinkCleanupHelper;
catch % Catch a file-transfer error and print some text in Matlab's Command Window
    fprintf('Problem receiving data file ''%s''\n', edfFile);
    EyelinkCleanupHelper;
    psychrethrow(psychlasterror);
end
