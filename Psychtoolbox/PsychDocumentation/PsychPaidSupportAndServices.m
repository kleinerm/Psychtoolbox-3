function PsychPaidSupportAndServices(mininag)
% PsychPaidSupportAndServices
%
% Fill me with life!
%

% Mini advert requested by some caller, e.g., PsychtoolboxPostInstallRoutine?
if exist('mininag', 'var') && (mininag > 0)
    fprintf('\n');

    % mininag 2 if called from an error handler, mininag 1 if called from
    % general setup code:
    if mininag == 2
        fprintf('NOTE: You may want to acquire paid priority support for future issues like this.\n');
    else
        fprintf('IMPORTANT NEWS:\n\n');
        fprintf('You can now financially contribute to Psychtoolbox sustainability, upkeep and continued\n');
        fprintf('improvement by buying a community membership, which also includes some priority support\n');
        fprintf('for questions regarding its use, or issues you may have with it. We also offer paid\n');
        fprintf('feature development and other useful commercial services.\n');
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

help PsychPaidSupportAndServices;
