function rc = PsychLicenseHandling(cmd, varargin)
% PsychLicenseHandling - Manage Psychtoolbox software licensing.
%
% This function facilitates the setup and administration of license
% management for Psychtoolbox on operating systems and platforms that
% require either a paid license or a time-limited free trial.
%
% Once license management is configured for your operating system and
% hardware, you can manage your license, including activation and
% deactivation on a machine, checking licensing status, and generating a
% support authentication token.
%
% For information on purchasing license keys for Psychtoolbox, visit:
%
% https://www.psychtoolbox.net
%
% The prebuilt, tested, and supported mex files included in Psychtoolbox
% are provided by our commercial host company, the Medical Innovations
% Incubator GmbH, as of December 2024. Contact details are as follows:
%
% Medical Innovations Incubator GmbH
% Eisenbahnstr. 63
% 72072 Tübingen
% Germany
%
% Commercial register: HRB 751684
% Register court: Local court Stuttgart, Germany
%
% For general and data protection inquiries, email: info@mi-incubator.com
% For Psychtoolbox questions, email: psychtoolbox@mi-incubator.com
%
% The data protection and privacy statements related to Psychtoolbox
% software license management and associated data collection and processing
% can be found at:
%
% https://www.psychtoolbox.net/data-protection
%
% Once a license has been activated online, you can use the Psychtoolbox
% mex files offline for a limited number of days without an active internet
% connection. After this offline period, the mex files will stop working
% until an internet connection is reestablished. During each startup,
% Psychtoolbox will inform you of the remaining days of offline use before
% your computer must connect to the internet while Psychtoolbox is running.
% Each successful connection to the license servers will reset the offline
% use period to its maximum duration. When connected to the internet,
% Psychtoolbox will sync with the server at the first use in a session and
% periodically every few hours.
%
% If your local environments firewall prevents online license management from
% working, see the section at the bottom for how to add firewall exception rules
% to make it work anyway. Below that section you will also find instructions for
% operating in a fully offline "air gapped" environment without any internet access.
% The latter may require specially configured licenses, as our default licenses do
% not support fully offline use.
%
% Subfunctions and their meaning:
% -------------------------------
%
% PsychLicenseHandling('Setup');
% - Informs the user about the purpose of license management, asks them for
% consent to enable license management, and performs initial setup to get
% to working mex files for license managed Psychtoolboxes. This function
% can be manually called by users, but by default it is called by the
% Psychtoolbox setup/install/update routines to automate license onboarding.
%
% PsychLicenseHandling('SetupLicense');
% - Like 'Setup', but always prompts for a new license key, even if a key
% is already enrolled, or a trial is active at the moment.
%
% PsychLicenseHandling('SetupGlobal');
% - This behaves mostly like 'Setup', but it is meant for system administrators
% or IT personnel to create a global configuration file to express consent to
% license management on behalf of all users of this Psychtoolbox installation,
% and also to store a license key for automatic node activation. The global
% configuration file will be stored in the Psychtoolbox main folder, ie. the
% folder printed by the PsychtoolboxRoot() function. You can use this function
% if you use disc imaging software or similar provisioning tools to install and
% setup a Psychtoolbox installation once as part of a provisioning image, then
% clone the provisioning image and Psychtoolbox installation to many physical
% machines. The global config file will cause each Psychtoolbox on each of the
% provisioned machines to activate and setup license management and activate that
% node at first use of Psychtoolbox.
%
% PsychLicenseHandling('Activate' [, licenseKeyOrUserCredential]);
% - Activate a paid license on a machine + operating system combination.
% This can either use a previously enrolled license key from earlier calls
% of PsychLicenseHandling('Activate') or PsychLicenseHandling('Setup'), or
% providing a new license key to activate the machine with a new license,
% by providing the new key in a string as optional 'licenseKeyOrUserCredential'
% parameter. Instead of a license key, one can also enter the login credentials
% of a user account with associated license key, to fetch the associated key
% automatically, e.g., if the account is jon.doe@doeland.us with password oink,
% then the credentials would be the following text: jon.doe@doeland.us:oink
%
% This function can also be used if your machine was offline longer than
% allowed and your local activation needs to be refreshed, or if your old
% license has expired, or been suspended for some reason, then been resumed
% or extended by you, and you need Psychtoolbox to refresh its view of the
% current licensing state. If things don't work for some licensing related
% reason, calling PsychLicenseHandling('Activate') may fix many problems.
%
% PsychLicenseHandling('Deactivate');
% - Deactivate a currently activated machine + operating system combination,
% for the purpose of freeing up one activation, so a different machine +
% operating system combination can be activated instead. This allows you to
% "shift around" which machines can be used if your license only allows a
% limited number of simultaneous activations on a limited number of
% machines, and you temporarily have more machines than available
% activations.
%
% PsychLicenseHandling('WipeLicense');
% - Deactivate a license on a machine and wipe the license key permanently
% if you don't intend to use that license on this machine anymore, e.g., if
% you are about to decomission a machine, or no longer use that license.
%
% active = PsychLicenseHandling('IsLicensed' [, feature]);
% - Check if this installation of Psychtoolbox currently has a valid and
% active license, and possibly print some information about the licensing
% status, e.g., license expiry date, or how long the machine can still be
% used offline. If the optional 'feature' parameter is provided as a name
% string, then checks if that specific subfeature is enabled by the
% currently active license.
%
% PsychLicenseHandling('AuthenticationToken');
% - Print an authentication token which allows you to prove that your
% machine has a properly paid, valid and active license associated with it.
% Our professional support personnel may ask you to provide such a token in
% some cases.
%
% PsychLicenseHandling('News');
% - Print latest stored news about Psychtoolbox, and also specifically related to
% this license and activations. Psychtoolbox prints those messages automatically
% once when they are new. This function will print them even if they have been
% printed before.
%
%
% USE IN STRICTLY FIREWALLED ENVIRONMENTS:
% ----------------------------------------
%
% If your firewall is blocking internet connections to our license servers,
% you can configure it as follows to allow connecting to the license servers.
% Follow the most recent instructions on this website for passthrough for EU data centers:
%
% https://docs.cryptlex.com/node-locked-licenses/proxies-and-firewall#for-our-eu-data-center
%
% As of March 2025, the following configuration would be needed, but check above
% website for up to date informations if in doubt:
%
% The following IP addresses and URL should be whitelisted:
%
%    IP Addresses to Whitelist:
%        75.2.113.112
%        99.83.149.57
%
%    Web API URL to Whitelist:
%        https://api.eu.cryptlex.com:443
%
% OFFLINE USE IN AIR GAPPED ENVIRONMENTS:
% ---------------------------------------
%
% Some non-standard software subscription licenses allow offline activation and
% deactivation by use of the customer portal and passing forth and back offline
% activation and deactivation request and response files. This allows use in air-
% gapped environments without access to the public internet or to our license
% servers. If your purchased license supports this, the functions are as follows:
%
% PsychLicenseHandling('ActivateEnrolledKeyOffline', pathToOfflineRequestResponseFile);
% - Either create an offline activation request file under the specified path/filename,
% which allows creation of an offline activation response file in the customer portal,
% or reads such an offline activation response file and activates your local machine.
%
% E.g., after enrolling a license key via PsychLicenseHandling('Setup') or
% PsychLicenseHandling('Activate', licenseKey); do the following:
%
% 1. PsychLicenseHandling('ActivateEnrolledKeyOffline', 'offlineRequest.dat');
%
% 2. Login to customer portal and upload 'offlineRequest.dat' to create offline
%    response file, downloaded to the file 'offlineResponse.dat'.
%
% 3. PsychLicenseHandling('ActivateEnrolledKeyOffline', 'offlineResponse.dat') to
%    activate this machine.
%
% PsychLicenseHandling('DeactivateEnrolledKeyOffline', pathToOfflineProofFile);
% - Deactivate the machine locally and write a deactivation proof file into the
% path/filename 'pathToOfflineProofFile'. You can upload that proof file into
% the customer portal to deactivate the machine in the license servers, so the
% machine activation that has been freed up can be reused on a different machine.
% Not all licenses allow offline deactivation of once activated machines.
%

% History:
%
% 23-Nov-2024   mk  Written as part of Mario Kleiners employment at the
%                   Medical Innovations Incubator GmbH in Tuebingen, Germany.
% 13-Jan-2025   mk  Add auto download and install for LM libraries if they
%                   are missing.
%
% 28-Feb-2025   mk  Improve auto download, make callable from external code,
%                   e.g., PsychtoolboxPostInstallRoutine().
%
% 26-Mar-2025   mk  Add offline "air-gapped" activation support.
% 27-Mar-2025   mk  Add 'News' function for printing of "push messages".

persistent forceReenterKey;

if nargin < 1 || isempty(cmd) || ~ischar(cmd)
    error('Command argument missing.');
end

rc = 0;

% Check if LexActivator client library installed? Install if needed.
if strcmpi(cmd, 'CheckInstallLMLibs')
    % LexActivator client library installed?
    if ~IsARM
        % Get rid of ARM libs in search path on non-ARM:
        warning off;
        rmpath([PsychtoolboxRoot 'PsychBasic/PsychPlugins/ARM64']);
        warning on;
    end

    if (~IsLinux && (~exist('libLexActivator.dylib', 'file') || ~exist('LexActivator.dll', 'file'))) || ...
       (IsLinux && ~IsOctave && ~exist('libLexActivator.so', 'file'))
        % Nope. Try to download and install them. This will happen for a
        % PTB checked out from our git repository or extracted from the
        % full source code zip files, or from Matlab Add-On manager,
        % instead of standard "Psychtoolbox folder only" zip file install.
        fprintf('License management support libraries are missing for some reason. Trying to download and install them now...\n');
        addpath([PsychtoolboxRoot '../managementtools']);
        downloadlexactivator(0, 1);

        % Doublecheck:
        if (~IsLinux && (~exist('libLexActivator.dylib', 'file') || ~exist('LexActivator.dll', 'file'))) || ...
           (IsLinux && ~IsOctave && ~exist('libLexActivator.so', 'file'))
            warning('Library installation FAILED! This will go badly soon...\n');
        else
            fprintf('Library installation success. Onwards!\n');
        end
    end

    return;
end

% LexActivator client library installed? Install if needed.
PsychLicenseHandling('CheckInstallLMLibs');

% Check if initial setup of license management is needed, and if so then do it:
if strcmpi(cmd, 'Setup') || strcmpi(cmd, 'SetupGlobal') || strcmpi(cmd, 'SetupLicense')
    % Check if this Psychtoolbox variant is requires license management at all:
    if ~WaitSecs('ManageLicense', 6)
        % Nope. Must be one of the free Linux variants, nothing to do:
        rc = 1;
        return;
    end

    % Needs license management. What's its general status?
    rc = WaitSecs('ManageLicense', 3);

    % Disabled due to missing user consent? Or global setup requested?
    if (rc == -1) || strcmpi(cmd, 'SetupGlobal')
        % Yes. We need to inform the user about the consequences of license
        % management, ask his permission to continue, then hopefully continue:
        more(pause('query'));
        fprintf('===============================================================================================================\n');
        fprintf('The included prebuilt mex files in this variant of Psychtoolbox need online license management to function.\n');
        fprintf('You cannot use Psychtoolbox, not even as a free time-limited trial, without enabling license management.\n\n');
        fprintf('These mex files are compiled from the publicly available Psychtoolbox source code, tested, and supported\n');
        fprintf('to a limited degree for licensed users by the "Medical Innovations Incubator GmbH" in Tübingen, Germany,\n');
        fprintf('the commercial host of Psychtoolbox. See ''help PsychLicenseHandling'' for contact information.\n\n');
        fprintf('Psychtoolbox and its prebuilt mex files are distributed in the hope that they will be useful, but\n');
        fprintf('WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n');
        fprintf('See the License.txt file in the Psychtoolbox root folder under ...\n\n');
        fprintf('%sLicense.txt\n\n', PsychtoolboxRoot);
        fprintf('... for more details.\n\n');
        fprintf('You will now be asked to allow or deny the use of this license management. If you decide not to allow it,\n');
        fprintf('then your journey ends here, as Psychtoolbox will deny any further operation.\n\n');
        fprintf('If you do allow it, Psychtoolbox will contact its internet license management servers to try to\n');
        fprintf('enable use on this specific combination of operating system and computer hardware, either as a\n');
        fprintf('time-limited free trial, or for a longer term as part of a properly paid license.\n\n');
        fprintf('Software license keys can be purchased according to the instructions on the following website:\n\n');
        fprintf('https://www.psychtoolbox.net\n\n');
        fprintf('Once license management has been successfully enabled, Psychtoolbox will periodically contact the\n');
        fprintf('license servers of our licensing service provider to validate the proper licensing status.\n\n');
        fprintf('During online communication with the servers, certain information is transmitted, processed, and\n');
        fprintf('stored on the servers about your hardware and software setup for the purpose of license validation.\n');
        fprintf('Some of the information may also be aggregated by us for statistical purposes, to guide decisions\n');
        fprintf('about future development and maintenance of Psychtoolbox, e.g., to decide which hardware setups,\n');
        fprintf('operating system versions, and versions of Matlab or Octave we should support, and to prioritize\n');
        fprintf('development and maintenance of existing and future features.\n\n');
        fprintf('The following is a non-exhaustive list of information that may be stored on the license servers:\n');
        fprintf('The internet IP address and host name of your computer, a hashed machine digital fingerprint,\n');
        fprintf('the type and version of the operating system, the machine architecture of the main processor\n');
        fprintf('(e.g., 64-Bit Intel or 64-Bit ARM), the type and version of your Psychtoolbox runtime\n');
        fprintf('scripting environment (e.g., Matlab R2024b or Octave 9.2), the version and flavor of Psychtoolbox,\n');
        fprintf('and when Psychtoolbox last contacted the licensing servers for a licensing data sync.\n');
        fprintf('Depending on the specific type of license you bought, additional information may be stored.\n\n');
        fprintf('For a description of our data protection and use policy with respect to the use of a license-managed\n');
        fprintf('Psychtoolbox, our privacy policy, and your rights as a user, please read the information on the\n');
        fprintf('following web page, which also contains contact information if you have any further questions\n');
        fprintf('or requests:\n\n');
        fprintf('https://www.psychtoolbox.net/data-protection\n\n');
        fprintf('Once you feel sufficiently informed, please answer the following yes or no question to either enable\n');
        fprintf('license management and contact the licensing servers for the first time, or to back out of the use of\n');
        fprintf('Psychtoolbox now, before any information is sent to our license servers from this specific user account.\n');
        fprintf('===============================================================================================================\n');
        more off;

        answer = '';
        while length(answer) < 1 || ~ismember(answer(1), 'yn')
            answer = input('Do you consent to enabling license management via internet now? [yes or no] : ', 's');
        end

        if ~strcmpi(answer, 'yes')
            fprintf('You did not consent to enabling license management, so our journey ends here. Live long and prosper!\n');
            rc = 0;
            return;
        end

        if strcmpi(cmd, 'SetupGlobal')
            configfilepath = [PsychtoolboxRoot 'LMOpsAllowed.txt'];
            fprintf('\nGLOBAL LICENSING SETUP: Storing general consent and key in global config file: %s\n', configfilepath);
            if exist([PsychtoolboxConfigDir 'LMOpsAllowed.txt'], 'file')
                delete([PsychtoolboxConfigDir 'LMOpsAllowed.txt']);
            end
        else
            if exist([PsychtoolboxRoot 'LMOpsAllowed.txt'], 'file')
                error('A global consent and license key config file already exists, therefore can not do regular per-user setup.');
            end

            configfilepath = [PsychtoolboxConfigDir 'LMOpsAllowed.txt'];
        end

        % Consent given! Enable use of license management.
        fprintf('\n');
        fprintf('License management enabled, according to user consent.\n');
        [fid, errmsg] = fopen(configfilepath, 'w');
        if fid == -1
            error(['Creating license management user consent file failed! Error: ' errmsg]);
        end
        fclose(fid);

        % Requery if we have a license key enrolled:
        rc = WaitSecs('ManageLicense', 3);
    end

    % Assign activation and trial metadata for upload to license servers and
    % use for aggregate statistical purposes:
    UpdateMetadata;

    % Force license key enrollment for global setup:
    if strcmpi(cmd, 'SetupGlobal') || strcmpi(cmd, 'SetupLicense')
        forceReenterKey = 1;
    end

    % At this point, rc == 0 means license key enrolled, other rc means no
    % license key enrolled. The forceReenterKey flag can enforce taking the
    % "no key enrolled" path:
    if ~isempty(forceReenterKey) && (forceReenterKey == 1)
        rc = 1;
    end

    if rc ~= 0
        % No license key enrolled. Ask user if they want to enroll and
        % activate a license:
        fprintf('This machine does not yet have a valid product license key enrolled and activated.\n');
        fprintf('If you have a suitable key, e.g., as bought from\n\nhttps://psychtoolbox.net\n\n');
        fprintf('then you can enroll it now and activate the associated license on this machine.\n\n');
        fprintf('Or if you already have an account with associated license key, then you can enter\n');
        fprintf('your user accounts credentials, as email:password, for me to fetch the associated key.\n\n');
        fprintf('Either enter your key/credentials now, or just press ENTER to try to enable a free trial.\n\n');

        while rc
            clear WaitSecs;

            productKey = input('License key / credentials (or ENTER for free trial): ', 's');
            if ~isempty(productKey)
                % Product key provided. Try to enroll and activate:
                rc = WaitSecs('ManageLicense', 2, productKey);
                if rc == 54
                    % Special case: Invalid license key entered. Force
                    % reentry immediately to avoid getting stuck:
                    fprintf('\nThe key or credentials were invalid. You must reenter a new valid key/credentials right now.\n');
                    continue;
                end

                % Store the global file with the key if global setup requested:
                if strcmpi(cmd, 'SetupGlobal')
                    [fid, errmsg] = fopen(configfilepath, 'w');
                    if fid == -1
                        error(['Creating license management global consent and config file failed! Error: ' errmsg]);
                    end

                    % Store product key in global config file:
                    fprintf(fid, '%s', productKey);
                    fclose(fid);
                end
            else
                % No key provided. Try if a free trial can be used:
                fprintf('No key provided. Trying if I can activate a free time limited trial...\n');
                rc = 1 - WaitSecs('ManageLicense', 0);
            end

            % Success?
            if rc
                % Nope.
                answer = '';
                while length(answer) < 1 || ~ismember(answer(1), 'yn')
                    answer = input('Want to fix the reported problem and try again after fixing? [yes or no]: ', 's');
                end

                if answer(1) == 'n'
                    % Get us out of retry while-loop:
                    rc = 0;
                end
            end
        end

        % Query final activation status:
        rc = WaitSecs('ManageLicense', 0);
    else
        % License key is enrolled. Check licensing status:
        rc = WaitSecs('ManageLicense', 0);

        while rc == 0
            % Key enrolled, but license not successfully activated for this machine.
            fprintf('A license key is enrolled, but the license is not active on this machine.\n');
            answer = '';
            while length(answer) < 1 || ~ismember(answer(1), 'yn')
                answer = input('Want me to try activating this machine with the enrolled key? [yes or no]: ', 's');
            end

            if answer(1) == 'y'
                % Try to activate:
                rc = WaitSecs('ManageLicense', 1);
                if rc == 0
                    % Activation worked! Requery licensing status:
                    rc = WaitSecs('ManageLicense', 0);
                else
                    if rc == 54
                        % Invalid license key enrolled. Set flag to force key reentry:
                        fprintf('The enrolled key was invalid. You will need to enter a new valid key.\n');
                        forceReenterKey = 1;
                    else
                        forceReenterKey = 0;
                    end

                    % Failed. Does user want to retry the whole thing?
                    answer = '';
                    while length(answer) < 1 || ~ismember(answer(1), 'yn')
                        answer = input('Want to retry activating after fixing the above mentioned problem? [yes or no]: ', 's');
                    end

                    if answer(1) ~= 'y'
                        % User gives up. Break out of while retry loop:
                        rc = 1;
                    else
                        % User wants to retry the whole activation workflow:
                        clear WaitSecs;
                        fprintf('\nEncore une fois!\n');
                        PsychLicenseHandling('Setup');
                        clear WaitSecs;
                        rc = WaitSecs('ManageLicense', 0);
                    end
                end
            else
                % Break out of while retry loop:
                rc = 1;
            end
        end

        % Final query for current licensing status:
        clear WaitSecs;
        rc = WaitSecs('ManageLicense', 0);
    end

    if rc
        fprintf('Psychtoolbox license successfully activated and ready to rock!\n');
    else
        fprintf('Psychtoolbox license disabled on this machine. Further functionality will likely fail.\n');
    end

    return;
end

% Check if this Psychtoolbox variant requires license management at all:
if ~WaitSecs('ManageLicense', 6)
    % Nope. Must be one of the free Linux variants, nothing to do:
    rc = 1;

    fprintf('This Psychtoolbox variant does neither support nor require license management. Bye!\n');

    return;
end

% License management enabled?
if WaitSecs('ManageLicense', 3) == -1
    % Nope:
    rc = 1;

    fprintf('Run PsychLicenseHandling(''Setup'') first, to consent to license management, or I can not proceed. Bye!\n');

    return;
end

if strcmpi(cmd, 'Activate')
    % Assign activation and trial metadata for upload to license servers and
    % use for aggregate statistical purposes:
    UpdateMetadata;

    % Activate, either with a new license product key, or using an already
    % enrolled key:
    if ~isempty(varargin)
        rc = WaitSecs('ManageLicense', 2, varargin{1});
    else
        rc = WaitSecs('ManageLicense', 1);
    end

    return;
end

if strcmpi(cmd, 'ActivateEnrolledKeyOffline')
    % Assign activation and trial metadata for upload to license servers and
    % use for aggregate statistical purposes:
    UpdateMetadata;

    % Must have license key already enrolled:
    if WaitSecs('ManageLicense', 3)
        error('License key not yet enrolled. Please do this by calling the ''Activate'' or ''Setup'' functions first.');
    end

    % Must have path/filename of to-be-created offline activation request file or
    % already existing offline activation response file:
    if isempty(varargin)
        error('Required path and filename of offline activation file missing.');
    end

    fname = varargin{1};
    if ~ischar(fname) || isempty(fname)
        error('Passed parameter is not a string with path/filename of offline file.');
    end

    if ~exist(fname, 'file')
        % No such file. Create an offline activation request file:
        rc = WaitSecs('ManageLicense', 1, [], fname);
        if rc == 0
            fprintf('Wrote activation request file ''%s'' for this machine. You must now login to\n', fname);
            fprintf('your customer self-service portal (https://medical-innovations.customer.eu.cryptlex.com),\n');
            fprintf('go to the ''Activations'' section, and use the ''Create Offline'' button to enter your\n');
            fprintf('license key and then upload the activation request file. If your license has spare activations\n');
            fprintf('left, then the portal will allow you download of an offline activation response file.\n');
            fprintf('Your next step would be calling this function again, but passing in the path/filename of that\n');
            fprintf('downloaded offline activation response file. This will activate this copy of Psychtoolbox for use\n');
            fprintf('if your purchased license actually allows offline activation, something not guaranteed for most\n');
            fprintf('licenses by default.\n\n');
        else
            warning('Procedure failed for reasons given above.');
        end
    else
        % File exists and is presumably an offline activation response file. Give it a shot:
        fprintf('Trying to offline activate with the offline activation response file named:\n');
        fprintf('''%s''. Engage!\n\n', fname);
        rc = WaitSecs('ManageLicense', 1, [], fname);

        if rc == 0
            fprintf('Success! Please note you will have to repeat this offline procedure every time when or after\n');
            fprintf('the offline grace period ends, unless there is not any such period set for your license. See the\n');
            fprintf('output above.\n');
        else
            warning('Procedure failed for reasons given above.');
        end
    end

    return;
end

if strcmpi(cmd, 'Deactivate')
    rc = WaitSecs('ManageLicense', -1);
    return;
end

if strcmpi(cmd, 'WipeLicense')
    rc = WaitSecs('ManageLicense', -2);
    return;
end

if strcmpi(cmd, 'WipeMetadata')
    WipeMetadata;
    return;
end

if strcmpi(cmd, 'DeactivateEnrolledKeyOffline')
    UpdateMetadata;

    % Must have license key already enrolled:
    if WaitSecs('ManageLicense', 3)
        error('License key not yet enrolled. Please do this by calling the ''Activate'' or ''Setup'' functions first.');
    end

    % Must have path/filename of to-be-created offline deactivation proof file:
    if isempty(varargin)
        error('Required path and filename for to be written offline deactivation proof file missing.');
    end

    fname = varargin{1};
    if ~ischar(fname) || isempty(fname)
        error('Passed parameter is not a string with path/filename of offline file.');
    end

    if ~exist(fname, 'file')
        rc = WaitSecs('ManageLicense', -1, fname);
        if rc == 0
            fprintf('Successfully deactivated! The written offline deactivation proof file ''%s''\n', fname);
            fprintf('must now be uploaded to the customer self-service license management portal at\n');
            fprintf('https://medical-innovations.customer.eu.cryptlex.com . Login, go to the ''Activations''\n');
            fprintf('section, and press the ''Delete Offline'' button. Then enter your license key for this\n');
            fprintf('machine and upload the just written deactivation proof file, to deactivate this node\n');
            fprintf('on the license servers. This will free up one activation on the given license to allow\n');
            fprintf('you to reactivate a different operating system + machine combination.\n');
            fprintf('Please note that not all license plans allow you to offline deactivate nodes after their\n');
            fprintf('activation.\n\n');
        else
            warning('Procedure failed for reasons given above.');
        end

        return;
    else
        error('A file already exists under that path/filename. Doing nothing.');
    end
end

if strcmpi(cmd, 'IsLicensed')
    if ~isempty(varargin)
        rc = WaitSecs('ManageLicense', 0, varargin{1});
    else
        rc = WaitSecs('ManageLicense', 0);
    end

    return;
end

if strcmpi(cmd, 'AuthenticationToken')
    % Assign activation and trial metadata for upload to license servers and
    % use for aggregate statistical purposes:
    UpdateMetadata;

    % Print authentication token, and also sync to server first, to token
    % sync timestamp can be compared to timestamp of last sync in the
    % support web interface:
    rc = WaitSecs('ManageLicense', 4);

    return;
end

if strcmpi(cmd, 'News')
    % Print all news messages stored for Psychtoolbox globally, and for the given
    % license etc.:
    rc = WaitSecs('ManageLicense', 7);

    return;
end

end

function UpdateMetadata
    % Nothing to do at the moment, as MEX files do this already for
    % 'hostappversion' and 'machinemodel'.
    return;
end

function WipeMetadata
    WaitSecs('ManageLicense', 5, 'hostappversion', '');
    WaitSecs('ManageLicense', 5, 'machinemodel', '');
end
