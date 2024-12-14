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
% 72070 Tübingen
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
% PsychLicenseHandling('Activate' [, licenseKey]);
% - Activate a paid license on a machine + operating system combination.
% This can either use a previously enrolled license key from earlier calls
% of PsychLicenseHandling('Activate') or PsychLicenseHandling('Setup'), or
% providing a new license key to activate the machine with a new license,
% by providing the new key in a string as optional 'licenseKey' parameter.
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

% History:
%
% 23-Nov-2024   mk  Written as part of Mario Kleiners employment at the
%                   Medical Innovations Incubator GmbH in Tuebingen, Germany.
%

persistent forceReenterKey;

if nargin < 1 || isempty(cmd) || ~ischar(cmd)
    error('Command argument missing.');
end

rc = 0;

% Check if initial setup of license management is needed, and if so then do it:
if strcmpi(cmd, 'Setup')
    % Check if this Psychtoolbox variant is requires license management at all:
    if ~WaitSecs('ManageLicense', 6)
        % Nope. Must be one of the free Linux variants, nothing to do:
        rc = 1;
        return;
    end

    % Needs license management. What's its general status?
    rc = WaitSecs('ManageLicense', 3);

    % Disabled due to missing user consent?
    if rc == -1
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

        % Consent given! Enable use of license management.
        fprintf('\n');
        fprintf('License management enabled, according to user consent.\n');
        [fid, errmsg] = fopen([PsychtoolboxConfigDir 'LMOpsAllowed.txt'], 'w');
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
        fprintf('then you can enroll it now and activate the associated license on this machine.\n');
        fprintf('Either enter your key now, or just press ENTER to try to enable a free trial period.\n\n');

        while rc
            clear WaitSecs;

            productKey = input('License key (or ENTER for free trial): ', 's');
            if ~isempty(productKey)
                % Product key provided. Try to enroll and activate:
                rc = WaitSecs('ManageLicense', 2, productKey);
                if rc == 54
                    % Special case: Invalid license key entered. Force
                    % reentry immediately to avoid getting stuck:
                    fprintf('The key was invalid. You must reenter a new valid key right now.\n');
                    continue;
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
                        PsychLicenseHandling('SetupIfNeeded');
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
end

end

function UpdateMetadata
    % Assign runtime environment version to activation data:
    if IsOctave
        % Octave version number:
        WaitSecs('ManageLicense', 5, 'hostappversion', version);
    else
        % Matlab release name, e.g., R2024b:
        WaitSecs('ManageLicense', 5, 'hostappversion', version('-release'));
    end

    % Assign machine model code name to activation data on macOS:
    if IsOSX
        [rc, model] = system('sysctl -n hw.model');
        if rc == 0
            model = strtrim(model);
            WaitSecs('ManageLicense', 5, 'machinemodel', model);
        end
    end
end

function WipeMetadata
        WaitSecs('ManageLicense', 5, 'hostappversion', '');
        WaitSecs('ManageLicense', 5, 'machinemodel', '');
end
