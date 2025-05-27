function PsychPaidSupportAndServices(mininag)
%
% If you are using a Psychtoolbox variant and version that requires a paid
% software license key to function, e.g., on Microsoft Windows, or Apple
% macOS, or for Matlab on Linux, then a few minutes of basic support per
% year for basic questions may be included in the specific type of software
% license you bought. If at all, when, and how much support will be
% provided is at the sole discretion of the Psychtoolbox team. In any case,
% if you wanted to ask for this basic support, you'd have to present an
% authentication token by running this function, or by calling
% PsychLicenseHandling('AuthenticationToken').
%
% For paid support beyond that, on the user forum or GitHub issue tracker,
% you may have bought a "Psychtoolbox paid support membership" in the past.
% If that membership is still valid, then the following procedure would
% allow you to use it:
%
% 1. As part of your purchase, you will have received a document - an
%    invoice - with a printed "Order Id" or "Order no.", and a printed "License key".
%
% 2. Next you can run *this* function PsychPaidSupportAndServices in Octave or
%    Matlab. The function will ask you if you need paid support and do have
%    an active license key to prove your paid membership. Answer "y" for yes.
%
% 3. Next the function will ask you about your "Order Id" and "License key".
%    Please enter that info that you got from step 1.
%
% 4. The function will print out an "authentication token", a string of letters and
%    numbers. Now you can post your question on the Psychtoolbox user forum at
%    https://psychtoolbox.discourse.group or open a new issue for bugs and feature
%    requests on our GitHub issue tracker at the following URL:
%    https://github.com/Psychtoolbox-3/Psychtoolbox-3/issues
%
%    Please add the "authentication token" to your forum question or GitHub issue.
%    This will prove to us that you deserve paid support for your question or
%    issue. Please note that initial activation of your license key may take 5-10
%    days after date of receipt of payment, so do not buy the paid support last
%    minute, when the "house is already burning", but ahead of time, like you
%    would do with a fire insurance.
%

% Mini advert requested by some caller, e.g., PsychtoolboxPostInstallRoutine?
if exist('mininag', 'var') && (mininag > 0)
    fprintf('\n');

    % mininag 2 if called from an error handler, mininag 1 if called from
    % general setup code:
    if mininag == 2
        fprintf('NOTE: You may want to buy paid support for issues like this.\n');
    end

    fprintf('Please type ''PsychPaidSupportAndServices'' to learn more.\n');
    fprintf('\n\n');

    return;
end

% Nope, full request by user for information:
try
    fid = fopen([PsychtoolboxConfigDir 'welcomemsgdone'], 'w');
    fclose(fid);
catch
end

more on;
help PsychPaidSupportAndServices;
more off;

% Check if user wants to file a support request:
fprintf('\n\n');

try
    fprintf('Checking if this machine has a valid Psychtoolbox software license activated...\n\n');
    if ~PsychLicenseHandling('AuthenticationToken')
        fprintf('\nIf an authentication token was printed, you may be able to get a few minutes of\n');
        fprintf('free support. This would spare you from paying for additional paid support. Worth a try...\n\n');
    else
        fprintf('\nNo valid and active Psychtoolbox software license with support option detected. That leaves the paid option...\n\n');
    end
catch
    fprintf('No valid and active Psychtoolbox software license detected. That leaves the paid option...\n\n');
end

answer = '';
while length(answer)~=1 || ~ismember(answer, ['y', 'n'])
    answer = strtrim(input('Do you need additional paid support now and have an active support license key [y/n]? ', 's'));
end

if ~strcmpi(answer, 'y')
    fprintf('\nThen we can not help you. Bye.\n');
    return;
end

% Ok, our valued community member wants support now. Guide them through the process:
orderId = '';
while isempty(orderId)
    fprintf('\nThe Order Id is on the invoice you received, as "Order no." between the invoice\n');
    fprintf('number and the customer number.\n\n');

    orderId = strtrim(upper(input('Please enter the Order Id of your license, or just ENTER if you can not find it: ', 's')));
    if length(orderId) ~= 8 || ~all(isstrprop(orderId, 'alphanum'))
        fprintf('\nOrder Id seems to be invalid: It must be 8 letters or numbers, e.g., XS92UVY3\n');
        orderId = '';
    end

    if isempty(orderId)
        fprintf('\nIf you can not find the Order Id, i can also just use the first 8 characters of\n');
        fprintf('your license key. This poses a mildly higher risk of somebody guessing your key\n');
        fprintf('and abusing your community membership to their advantage, or of other mixups.\n\n');
        if lower(input('Do you want me to use the license key instead [y/n]? ', 's')) == 'y'
            break;
        end
    end
end

% Get the date and time string as 2nd component:
requestTimeDate = sprintf('%i', round(clock)); %#ok<CLOCK>

licenseKey = '';
while isempty(licenseKey)
    licenseKey = strtrim(upper(input('Please enter the license key: ', 's')));
    if length(licenseKey) ~= 35 || ~all(isstrprop(licenseKey([1:5, 7:11, 13:17, 19:23, 25:29, 31:35]), 'alphanum')) || ~strcmp(licenseKey([6, 12, 18, 24, 30]), '-----')
        fprintf('The license key seems to be invalid: It must be 30 letters or numbers in dash separated groups of five, e.g.,\n');
        fprintf('2BQR7-R5ZQP-SA36G-RAVDJ-ABZBM-PKKFJ\n\n');
        licenseKey = '';
    end
end

% User wants us to use first eight chars of license key as orderId?
if isempty(orderId)
    orderId = licenseKey(1:8);
end

% Assemble the to-be-hashed string:
publicString = [orderId '-' requestTimeDate];
hashSrcString = [publicString '-' licenseKey];

% Hash it with SHA-256:
% fprintf('\nThe following string will be SHA-256 hashed: %s\n\n', hashSrcString);
hashString = computeSHA256(hashSrcString);

% Assemble final token for public posting at user forum, issue tracker etc.:
authToken = [publicString ':' hashString];
fprintf('\n\nPlease add the following string to your request when\n');
fprintf('asking for support on the Psychtoolbox user forum\n');
fprintf('[ https://psychtoolbox.discourse.group ]\n');
fprintf('or on the Psychtoolbox-3 GitHub issue tracker\n');
fprintf('[ https://github.com/Psychtoolbox-3/Psychtoolbox-3/issues ]\n');
fprintf('to prove that you are eligible for paid support:\n');
fprintf('========================================================================================\n\n');
fprintf('%s\n\n', authToken);
fprintf('========================================================================================\n\n');

% Validation code for the above:
if 0
    % Disassemble into pieces:
    eOrderId = authToken(1:8) %#ok<*UNRCH>
    epublicStringHashSep = strfind(authToken, ':');
    epublicString = authToken(1:epublicStringHashSep-1)
    ehashString = authToken(epublicStringHashSep+1:end)
    % Here we'd use eOrderId to lookup licenseKey in our DB.
    % Then reassemble source string for hashing:
    ehashSrcString = [epublicString '-' licenseKey]
    % Hash it, and compare our local hash against the one provided in public authToken:
    bingo = strcmp(ehashString, computeSHA256(ehashSrcString))
end

return;

% Embedded SHA-256 implementation. This is the same as PsychComputeSHA(msg, '256'),
% but embedded here to make PsychPaidSupportAndServices() self-contained.
function rethash = computeSHA256(msg)
    shaId = '256';
    if exist('hash', 'builtin')
        % Octave builtin
        rethash = hash(['SHA' shaId], msg);
    else
        md = javaMethod('getInstance', 'java.security.MessageDigest', ['SHA-' shaId]);
        rethash = sprintf('%2.2x', typecast(md.digest(uint8(msg)), 'uint8')');
    end
return;
