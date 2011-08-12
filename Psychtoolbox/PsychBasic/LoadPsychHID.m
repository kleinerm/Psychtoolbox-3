function LoadPsychHID
% LoadPsychHID - Try to get PsychHID linked and loaded on MS-Windows, no
% matter what.

if ~IsWin
    % Nothing to do on non-Windows:
    return;
end

if IsWinMatlabR11Style
    % Unsupported. No-op:
    return;
end

% Windows on Octave or with Matlab R2007a or later. Give it a try:
try
    PsychHID('Version');
catch %#ok<CTCH>
    % PsychHID loading and linking failed:
    if IsWin
        % PsychHID failed on Windows. Most likely cause would be "invalid
        % MEX file error" due to PsychHID failing to link against
        % required DLL's. libusb-1.0 may not be properly installed?
        fprintf('INFO: Initial invocation of the PsychHID mex file failed!\n');
        fprintf('INFO: Most likely a required DLL is not installed in your system, e.g., libusb-1.0.dll\n');
        fprintf('INFO: I will now check if this is the culprit and work around it. To avoid future warnings,\n');
        fprintf('INFO: please copy the libusb-1.0.dll from the PsychContributed folder into your C:\\WINDOWS\\system32\\ \n');
        fprintf('INFO: folder or a similarly appropriate place. You can get a fresher copy of libusb-1.0.dll from \n');
        fprintf('INFO: http://libusb.org/wiki/windows_backend if you want to stay up to date.\n');
        fprintf('INFO: Retrying now, may fail...\n');

        % The old drill: cd into our PsychContributed folder which
        % contains required DLL's. Retry by self-invocation. If this was
        % the culprit, then the linker should load, link and init
        % PsychHID and we should succeed. Otherwise we fail again.
        wd = pwd;
        try
            cd([PsychtoolboxRoot 'PsychContributed' filesep ]);
            PsychHID('Version');
            cd(wd);
            return;
        catch %#ok<CTCH>
            cd(wd);
            psychrethrow(psychlasterror);
        end
    end

    % Game over.
    psychrethrow(psychlasterror);
end
