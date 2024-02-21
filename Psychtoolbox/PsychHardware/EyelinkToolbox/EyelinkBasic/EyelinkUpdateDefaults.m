function EyelinkUpdateDefaults(el)
% USAGE = EyelinkUpdateDefaults(el) 
%
% This function passes changes to the calibration defaults structure
% to the callback function. To be called after EyelinkInitDefaults if any
% changes are made to the output structure of that function, e.g.
%    el=EyelinkInitDefaults(window);
%    el.backgroundColour = BlackIndex(window);
%    EyelinkUpdateDefaults(el);
%
% 27-1-2011 NJ created 
% 19-12-2012 IA Fix hardcoded callback 

callStack = dbstack;
if ~isempty(el.callback) && exist(el.callback,'file')
    if ~isempty(el.calImageTargetFilename)
        if exist(el.calImageTargetFilename, 'file')
            el.calImageInfo = imfinfo(el.calImageTargetFilename); % Get image file info
            el.calImageData = imread(el.calImageTargetFilename); % Read image from file
            el.calImageTexture = Screen('MakeTexture', el.window, el.calImageData); % Convert image file to texture
            if ~strcmpi(el.calTargetType, 'image')
                warning(sprintf([ ...
                    'EyelinkToolbox -- ''el.calImageTargetFilename'' is configured but will not be used until\n' ...
                    '\tel.calTargetType = ''image''\n' ...
                    'is also set and \n' ...
                    '\tEyelinkUpdateDefaults(el)\n' ...
                    'is called again thereafter.\n\n']));
            end
        else
            warning(sprintf([ ...
                'EyelinkToolbox -- File Not Found:\n', ...
                '\tel.calImageTargetFilename = %s\n\n'], ... 
                el.calImageTargetFilename));
        end
    end

    if ~isempty(el.calAnimationTargetFilename)
        if exist(el.calAnimationTargetFilename, 'file')
            if strcmpi(el.calTargetType, 'video')
                if el.targetbeep ~= 0
                   warning(sprintf([ 'EyelinkToolbox - ''el.calAnimationTargetFilename'' set for video, but\n' ...
                       'el.targetbeep not set == 0 and may cause playback issues (freezing).\n']));
                end

                if el.feedbackbeep ~= 0
                   warning(sprintf([ 'EyelinkToolbox - ''el.calAnimationTargetFilename'' set for video, but\n' ...
                       'el.feedbackbeep not set == 0 and may cause playback issues (freezing).\n']));
                end
            else
                warning(sprintf([ ...
                    'EyelinkToolbox -- ''el.calAnimationTargetFilename'' is configured but will not be used until\n' ...
                    '\tel.calTargetType = ''video''\nis also set and \n\tEyelinkUpdateDefaults(el)\nis called again thereafter.\n\n']));
            end
        else
            warning(sprintf([ ...
                'EyelinkToolbox -- File Not Found:\n', ...
                '\tel.calAnimationTargetFilename = %s\n\n'], ...
                el.calAnimationTargetFilename));
        end
    end

    if (el.feedbackbeep || el.targetbeep) && ~any(strcmpi({callStack.name}, 'EyelinkInitDefaults'))
        if isempty(el.ppa_pahandle) && strcmp('PsychEyelinkDispatchCallback', el.callback)
            if PsychPortAudio('GetOpenDeviceCount') > 0
                warning(sprintf(['EyelinkToolbox -- el.feedbackbeep or el.targetbeep are set requiring audio playback.\n' ...
                    'While a PsychPortAudio device has already been opened, no device handle was passed for EyeLink audio\n' ...
                    'feedback to use.\n'...
                    'See ''help SR-ResearchDemos'' projects for implementation examples.\n' ...
                    '***Disabling EyeLink audio feedback to avoid conflicts with PsychPortAudio device previously opened***\n' ...
                    '\tel.feedbackbeep = 0\n\tel.targetbeep = 0']));
                el.feedbackbeep = 0;
                el.targetbeep = 0;
            else
                warning(sprintf(['EyelinkToolbox -- el.feedbackbeep or el.targetbeep are set requiring audio\n' ...
                    'playback, but no open PsychPortAudio devices have been passed using el.ppa_pahandle.\n' ...
                    'See ''help SR-ResearchDemos'' projects for implementation examples.\n' ...
                    '... Opening default audio device with PsychPortAudio automatically.\n' ...
                    'To otherwise disable EyeLink audio feedback altogether, set el.feedbackbeep &\n' ...
                    'el.targetbeep to 0.']));

                % Force-close potential old Snd() device:
                Snd('Close', 1);

                % Initialize, in case it hasn't been done already:
                InitializePsychSound();

                % Open with exactly the parameters a standard Snd('Open') would use:
                el.ppa_sndhandle = PsychPortAudio('Open', [], 1, 0, [], 2);

                % Attach to Snd() for permanent use:
                Snd('Open', el.ppa_sndhandle, 1);
            end
        end
    end

    % evaluate the callback function with the new el defaults
    feval(el.callback, el);
end
end
