function PsychtoolboxRegistration(isUpdate, flavor)
% PsychtoolboxRegistration(isUpdate, flavor) - Online registration.
%
% This function is used to register your working copy of Psychtoolbox
% with the official www.psychtoolbox.org website. The routine is
% normally called by PsychtoolboxPostInstallRoutine at the end of
% each successfull update or initial download of a copy of Psychtoolbox.
%
% The routine is fail-safe in the sense that it will not prevent you
% from running PTB if online registration fails for some reason.
%
% The routine transmits a bit of information about your copy of PTB and
% your system environment to the website, together with a world-wide unique
% identifier of your computer+operating system. The unique identifier is
% the MAC address of your computers primary network adapter. We need this
% to disambiguate multiple downloads/updates from the same user so we
% do not count the same system setup multiple times in our statistics.
% We have no way to easily find out who you are based on this information and
% could not care less about that information.
%
% We collect this information exclusively for the following purpose:
%
% 1. Statistics about total number of downloads for the purpose of
%    documenting the use of PTB.
%
% 2. Statistics about distribution of user base wrt. operating system,
%    and Matlab / Octave version to prioritize development for the most
%    common platform+OS combinations.
%
%
% Technical Notes:
% The minimal network server for collecting the transmitted information would be the
% following line of code, executed in the Linux/OS-X command window:
%
% while true; do nc -l -p 2000 | head -c 200 >> /tmp/testlog ; done
%
% This will receive data from network port 2000 (any number between 1025 and 65535
% is valid), discard everything except the first 200 bytes (to prevent overflows due
% to bugs or hacker attacks) and append it to the logfile /tmp/testlog, one
% line per registered copy of Psychtoolbox.

% History:
% 18.08.2006 Written (MK)

% Address and port number of our statistics server:
ptbserveraddress = 'kumquat.kyb.local 2000';

% Running under OpenGL-PTB? Otherwise we abort.
AssertOpenGL;

try
    
    % All arguments supplied? Otherwise we skip registration.
    if nargin < 2
        fprintf('PsychtoolboxRegistration: WARNING - Call arguments missing. Skipped...\n');
        return;
    end;
    
    % Query MAC address as unique machine id:
    compinfo = Screen('Computer');
    if isfield(compinfo, 'MACAddress')
        mac = compinfo.MACAddress;
    else
        % Unknown MAC address. Use our dummy value:
        mac = '00:00:00:00:00:00';
    end
    
    % Default path and name for netcat command:
    nccommand = 'nc';
    
    % Query OS-Type:
    if IsOSX
        ostype = 'MacOS-X';
        osversion = compinfo.system;
    end
    
    if IsWin
        ostype = 'Windows';
        osversion = 'Unknown';
        % Define path to our own netcat executable for M$-Windows:
        nccommand = [PsychtoolboxRoot 'PsychContributed\nc '];
    end
    
    if IsLinux
        ostype = 'LinuxOS';
        osversion = 'Unknown';
    end
    
    % Query runtime environment:
    if IsOctave
        runtimeenv = 'Octave';
        runtimeversion = 'Unknown';
    else
        runtimeenv = 'Matlab';
        runtimeversion = version;
    end
    
    % Build unique id string for this system:
    uniqueID = [mac '-' ostype '-' runtimeenv '-' runtimeversion '-' flavor '-' num2str(isUpdate) '-' date];
    
    fprintf('Online Registration: Will try to transmit the following string of data\n');
    fprintf('to the www.psychtoolbox.org website for statistical purpose:\n');
    fprintf('%s\n\n', uniqueID);
    fprintf('Type ''help PsychtoolboxRegistration'' at the Matlab/Octave command prompt\n');
    fprintf('to learn about the purpose and scope of online registration.\n\n');
    fprintf('This can take up to 10 seconds... The system reports:\n');
    
    % Execute transmission command: We time out after 10 seconds if it does not work.
    rc = system(['echo "' uniqueID '" | ' nccommand ' -w 10 -v ' ptbserveraddress ' ']);
    fprintf('\n');
    
    % Did it work?
    if rc==0
        fprintf('Success! Thanks for online registration of this copy.\n');
    else
        fprintf('Failed! Anyway, this does not affect your use of PTB in any way.\n');
    end
catch
    % This empty try-catch-end framework is just to make absolutely sure that our
    % installation procedure does not fail due to some error in the online registration
    % script.
end

% We are done.
return;
