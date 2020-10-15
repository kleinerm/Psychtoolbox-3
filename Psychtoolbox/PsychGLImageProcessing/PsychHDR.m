function varargout = PsychHDR(cmd, varargin)
% PsychHDR - Support and control stimulus display to HDR "High dynamic range" displays.
%
% This driver provides both, the setup code for the HDR main setup command
% PsychImaging('AddTask', 'General', 'EnableHDR'), to get HDR display enabled
% for a specific Psychtoolbox onscreen window on a specific HDR display, and
% utility subfunctions for user-scripts to call after initial setup, to modify
% HDR display behaviour.
%
% HDR currently only works on graphics card + operating system combinations which
% support both the OpenGL and Vulkan rendering api's and also efficient OpenGL-Vulkan
% interoperation. Additionally, the Vulkan driver, graphics card, and your display
% device must support at least the HDR-10 standard for high dynamic range display.
%
% As of October 2020, these graphics cards would be suitable:
%
% - Modern AMD (RX 500 "Polaris" and later recommended) and NVidia (GeForce 1000
%   "Pascal" and later recommended) graphics cards under a Windows-10 system, which
%   is up to date for the year 2020.
%
% - Modern AMD graphics cards (like above) under modern GNU/Linux (Ubuntu 18.04.4-LTS
%   at a minimum (untested!), or better Ubuntu 20.04-LTS and later recommended), with
%   the AMD open-source Vulkan driver "amdvlk". Install driver release 2020-Q3.5 from
%   September 2020, or later versions. This webpage has amdvlk download and installation
%   instructions:
%
%   https://github.com/GPUOpen-Drivers/AMDVLK/releases
%
% Most often you won't call this function directly, but Psychtoolbox will call
% it appropriately from the PsychImaging() function. Read relevant sections
% related to 'EnableHDR' in 'help PsychImaging' first, before venturing into the
% subfunctions offered by this function.
%
% User accessible commands and their meaning:
% -------------------------------------------
%
% oldVerbosity = PsychHDR('Verbosity' [, verbosity]);
% - Returns and optionally sets level of 'verbosity' for driver debug output.
%   'verbosity' = New level of verbosity: 0 = Silent, 1 = Errors only, 2 = Warnings,
%   3 = Info, 4 = Debug, 5 -- ... = Higher debug levels.
%
%
% isSupported = PsychHDR('Supported');
% - Returns if HDR visual stimulus display is in principle supported on this setup.
%   1 = Supported, 0 = No driver, hardware or operating system support.
%
%
% hdrProperties = PsychHDR('GetHDRProperties', window);
% - Returns hdrProperties, a struct with information about the HDR display properties
%   of the onscreen window 'window'. Most importantly, it returns information about the
%   native color gamut of the HDR display device and its brightness range. The following
%   fields in hdrProperties exist at least:
%
%   'Valid'   Are the HDR display properties valid? 0 = No, as no data could be
%             queried from the display, 1 = Yes, data has been queried from display and is
%             supposed to represent actual display HDR capabilities and properties.
%
%   'HDRMode' 0 = None (SDR), 1 = Basic HDR-10 enabled with BT-2020 color space, 10
%             bpc color precision, and ST-2084 PQ Perceptual Quantizer EOTF.
%
%   'LocalDimmingControl' 0 = No, 1 = Local dimming control supported.
%
%   'MinLuminance' Minimum supported luminance in nits.
%
%   'MaxLuminance' Maximum supported peak / burst luminance in nits. This is often
%                  only achievable for a small area of the display surface over extended
%                  periods of time, e.g., only for 10% of the pixel area. The full display
%                  may only be able to sustain that luminance for a few seconds.
%
%   'MaxFrameAverageLightLevel' Maximum sustainable supported luminance in nits.
%
%   'MaxContentLightLevel' Maximum desired content light level in nits.
%
%   'ColorGamut' A 2-by-4 matrix encoding the CIE-1931 2D chromaticity coordinates of the
%                red, green, and blue color primaries in columns 1, 2 and 3, and
%                the white-point in column 4.
%
%
% oldlocalDimmmingEnable = PsychHDR('HDRLocalDimming', window [, localDimmmingEnable]);
% - Return and/or set HDR local backlight dimming enable setting for display
%   associated with window 'window'.
%
%   This function returns the currently set HDR local backlight dimming setting for
%   dynamic contrast control on the HDR display monitor associated with window
%   'window'.
%
%   Return argument 'oldlocalDimmmingEnable' is the current setting.
%   The optional 'localDimmingEnable' is the new setting to apply. This will only
%   work if the display and display driver supports the VK_AMD_display_native_hdr
%   Vulkan extension. As of July 2020, only "AMD FreeSync2 HDR compliant" HDR
%   monitors under Windows-10 with an AMD graphics card in fullscreen mode support
%   this.
%
%   The hdrProperties = PsychHDR('GetHDRProperties', window); function allows to query
%   if the current setup supports this function. Please note that this function will
%   always report the selected 'localDimmingEnable' setting made by your code on a
%   nominally supported setup. There is no way for our driver to detect if the mode
%   change on the display was accepted, as the operating system provides no feedback
%   about this. At least one model of "compatible" monitor is already known to
%   ignore this setting, unknown if this is an AMD driver bug or monitor firmware
%   bug. Tread carefully! Manual control of this setting on the monitor itself may
%   be the safer choice.
%
%
% a) oldHdrMetadata = PsychHDR('HDRMetadata', window, metadataType [, maxFrameAverageLightLevel][, maxContentLightLevel][, minLuminance][, maxLuminance][, colorGamut]);
% b) oldHdrMetadata = PsychHDR('HDRMetadata', window [, newHdrMetadata]);
% - Return and/or set HDR metadata for presentation window 'window'.
%
%   This function returns the currently defined HDR metadata that is sent
%   to the HDR display associated with the window 'window'. It optionally
%   allows to define new HDR metadata to send to the display, starting with
%   the next presented visual stimulus image, ie. the successfull completion
%   of the next Screen('Flip').
%
%   The mandatory parameter 'metadataType' specifies the format in which
%   HDR metadata should be returned or set.
%
%   Return argument 'oldHdrMetadata' is a struct with information about the
%   current metadata. Optionally you can define new metadata to be sent to
%   the display in one of the two formats a) or b) shown above: Either a)
%   as separate parameters, or b) as a 'newHdrMetadata' struct. If you use
%   the separate parameters format a) and specify any new settings, but
%   omit some of the optional parameter values or leave them [] empty, then
%   those values will remain at their current / old values. If you use the
%   struct format b), then you must pass in a non-empty 'newHdrMetadata'
%   struct which contains the same fields as the struct returned in
%   'oldHdrMetadata', with all fields for the given 'MetadataType' properly
%   defined, otherwise an error will occur. Format b) is useful as a
%   convenience for querying 'oldHdrMetadata', then modifying some of its
%   values, and then passing this modified variant back in as
%   'newHdrMetadata'. For HDR movie playback, Screen('OpenMovie') also
%   optionally returns a suitable hdrStaticMetaData struct in the right
%   format for passing it as 'newHdrMetadata'.
%
%   The following fields in the struct and as new settings are defined:
%
%   'MetadataType' Type of metadata to send or query. Currently only a
%   value of 0 is supported, which defines "Static HDR metadata type 1", as
%   used and specified by the HDR standards CTA-861-3 / CTA-861-G (content
%   light levels) and SMPTE 2086 (mastering display color properties, ie.
%   color volume).
%
%   The content light level properties 'MaxFrameAverageLightLevel' and
%   'MaxContentLightLevel' default to 0 at startup, which signals to the
%   display device that they are unknown, a reasonable assumption for
%   dynamically rendered content with prior unknown maximum values over a
%   whole session.
%
%   'MaxFrameAverageLightLevel' Maximum frame average light level of the visual
%   content in nits, range 0 - 65535 nits.
%
%   'MaxContentLightLevel' Maximum light level of the visual content in
%   nits, range 0 - 65535 nits.
%
%   The following mastering display properties (~ color volume) default to
%   the properties of the connected HDR display monitor for presentation, if
%   they could be queried from the connected monitor. It is advisable to
%   override them with the real properties of the mastering display, e.g.,
%   for properly mastered movie content or image files where this data may
%   be available.
%
%   'MinLuminance' Minimum supported luminance of the mastering display in
%   nits, range 0 - 6.5535 nits.
%
%   'MaxLuminance' Maximum supported luminance of the mastering display in
%   nits, range 0 - 65535 nits.
%
%   'ColorGamut' A 2-by-4 matrix encoding the CIE-1931 2D chromaticity
%   coordinates of the red, green, and blue color primaries in columns 1,
%   2, and 3, and the location of the white-point in column 4. This defines
%   the color space and gamut in which the visual content was produced.
%

% History:
% 10-Jul-2020   mk  Written.

global GL;
persistent verbosity;

if nargin < 1 || isempty(cmd)
  help PsychHDR;
  return;
end

if isempty(verbosity)
    verbosity = PsychVulkan('Verbosity');
end

% oldVerbosity = PsychHDR('Verbosity' [, verbosity]);
if strcmpi(cmd, 'Verbosity')
    varargout{1} = verbosity;

    if (length(varargin) > 0) && ~isempty(varargin{1})
        verbosity = varargin{1};
        PsychVulkan('Verbosity', verbosity);
    end

    return;
end

% isSupported = PsychHDR('Supported');
if strcmpi(cmd, 'Supported')
    try
        % Enumerate all Vulkan gpu's, try to find if at least one
        % supports HDR. Return 1 == Supported if so, 0 == Unsupported otherwise.
        varargout{1} = 0;
        if PsychVulkan('Supported')
            devices = PsychVulkan('GetDevices');
            for d = devices
                if d.SupportsHDR
                    varargout{1} = 1;
                    break;
                end
            end
        end
    catch
        lasterror('reset');
        varargout{1} = 0;
    end

    return;
end

% hdrImagingModeFlags = PsychHDR('GetClientImagingParameters', hdrArguments);
if strcmpi(cmd, 'GetClientImagingParameters')
    % Parse caller provided EnableHDR task parameters into an easily usable
    % hdrArgs settings struct, while validating user input:
    hdrArgs = parseHDRArguments(varargin{1});

    % We need the final output formatting imaging pipeline chain to attach our
    % HDR post-processing shader, at a minimum for OETF mapping, e.g., PQ. Also
    % mark the onscreen window as HDR window, so texture precision for 'MakeTexture'
    % et al. can be adapted, and the movie playback engine can decode/remap video
    % frames from movies accordingly:
    hdrImagingModeFlags = mor(kPsychNeedOutputConversion, kPsychNeedHDRWindow);

    varargout{1} = hdrImagingModeFlags;
    return;
end

% [useVulkan, vulkanHDRMode, vulkanColorPrecision, vulkanColorSpace, vulkanColorFormat] = PsychHDR('GetVulkanHDRParameters', win, hdrArguments);
if strcmpi(cmd, 'GetVulkanHDRParameters')
    % Onscreen window handle:
    win = varargin{1};

    % Parse caller provided EnableHDR task parameters into an easily usable
    % hdrArgs settings struct, while validating user input:
    hdrArgs = parseHDRArguments(varargin{2});

    % useVulkan = 1, as we currently only do HDR via Vulkan/WSI:
    varargout{1} = 1;

    % vulkanHDRMode:
    varargout{2} = hdrArgs.hdrMode;

    % vulkanColorPrecision = 0 -- Derive from vulkanHDRMode by default:
    varargout{3} = 0;

    % vulkanColorSpace = 0 -- Derive from vulkanHDRMode by default:
    varargout{4} = 0;

    % vulkanColorFormat = 0 -- Derive from vulkanHDRMode by default:
    varargout{5} = 0;

    return;
end

% [hdrShader, hdrShaderIdString] = PsychHDR('PerformPostWindowOpenSetup', win, hdrArguments, icmShader, icmShaderIdString);
if strcmpi(cmd, 'PerformPostWindowOpenSetup')
    % Setup operations after Screen's PTB onscreen window is opened, and OpenGL and
    % the imaging pipeline are brought up, as well as the corresponding Vulkan/WSI
    % window. Needs to hook up the imaging pipeline to our HDR post-processing shaders.

    % Must have global GL constants:
    if isempty(GL)
        varargout{1} = 0;
        warning('PTB internal error in PsychHDR: GL struct not initialized?!?');
        return;
    end

    % Psychtoolbox Screen onscreen window handle:
    win = varargin{1};

    % Parse caller provided EnableHDR task parameters into an easily usable
    % hdrArgs settings struct, while validating user input:
    hdrArgs = parseHDRArguments(varargin{2});

    % hdrMode: 0 = SDR, 1 = HDR-10:
    hdrMode = hdrArgs.hdrMode;

    % Retrieve actual parameters of Vulkan window:
    hdrWindowProps = PsychVulkan('GetHDRProperties', win);

    % hdrWindowProps provides us with the actual output colorspace, framebuffer
    % precision, and if it is unorm or float.

    % TODO: Potentially adapt type of shader to use or add colorspace conversion
    % if source and target colorspace are different...

    % Get ICM color correction shader and shader name to use / attach to our HDR shader program:
    icmshader = varargin{3};
    icmstring = varargin{4};

    % HDR emulation dummy mode requested?
    if hdrMode == 0 && hdrArgs.dummy
        % For purpose of setup code in this subfunction, treat it as hdrMode 1:
        hdrMode = 1;
    end

    switch (hdrMode)
        case 1
            % HDR-10: At least 10 bpc color depth, BT-2020/Rec-2020 color space,
            % SMPTE ST-2084 PQ "Perceptual Quantizer" OETF encoding by us.

            if verbosity >= 3
                if ~hdrArgs.dummy
                    % The real thing:
                    fprintf('PsychHDR-INFO: HDR-10 output activated. BT-2020 color space, PQ EOTF. Unit is %s.\n', hdrArgs.inputUnit);
                else
                    % Only minimal emulation:
                    fprintf('PsychHDR-INFO: HDR-10 output EMULATION ON SDR DISPLAY activated. BT-2020 color space, PQ EOTF. Unit is %s.\n', hdrArgs.inputUnit);
                    fprintf('PsychHDR-INFO: This is only a most bare-bones emulation. VISUAL STIMULI WILL DISPLAY WRONG!\n');
                end
            end

            % Select scalefactor from user framebuffer to shader input:

            % Input scaling from input unit to 0 - 1 range, where 1 = 10000 nits:
            scalefactor = hdrArgs.inputScalefactor;

            % Set scaling factors for mapping SDR or HDR image and movie content into our HDR linear color space,
            % using the correct unit of luminance. Used by movie playback and Screen('MakeTexture') among others:
            Screen('HookFunction', win, 'SetHDRScalingFactors', [], hdrArgs.contentSDRToHDRFactor, 1 / scalefactor);

            % Load PQ shader:
            oetfshader = LoadGLSLProgramFromFiles('HDR10-PQ_Shader', [], icmshader);

            % Set it up - Assign texture image unit 0 and input values
            % scalefactor:
            glUseProgram(oetfshader);
            glUniform1i(glGetUniformLocation(oetfshader, 'Image'), 0);
            glUniform1f(glGetUniformLocation(oetfshader, 'Prescale'), scalefactor);
            glUseProgram(0);

            % Assign shader name: Add name of color correction shader:
            oetfshaderstring = sprintf('HDR-OETF-PQ-Formatter: %s', icmstring);

            % Shader is ready for OETF mapping.
        otherwise
            error('Unknown hdrMode %i - Implementation bug?!?', hdrMode);
    end

    % Return formatting shader to caller:
    varargout{1} = oetfshader;
    varargout{2} = oetfshaderstring;

    return;
end

% PsychHDR('HDRMetadata', window, hdrmetadatastruct) ?
if strcmpi(cmd, 'HDRMetadata') && (length(varargin) == 2) && isstruct(varargin{2})
    % This is a set call for HDRMetadata, with a struct providing the
    % parameters. Unpack the struct and convert into a conventional call to
    % PsychVulkanCore('HDRMetadata'):
    window = varargin{1};
    meta = varargin{2};
    [ varargout{1:nargout} ] = PsychVulkan(cmd, window, meta.MetadataType, meta.MaxFrameAverageLightLevel, meta.MaxContentLightLevel, meta.MinLuminance, meta.MaxLuminance, meta.ColorGamut);

    return;
end

% Dispatch subfunctions with "HDR" in their name to PsychVulkan(), which may
% hand them off to PsychVulkanCore():
if ~isempty(strfind(lower(cmd), 'hdr')) %#ok<*STREMP>
    if length(varargin) > 0
        [ varargout{1:nargout} ] = PsychVulkan(cmd, varargin{1:end});
    else
        PsychVulkan(cmd);
    end

    return;
end

sca;
error('Invalid command ''%s'' specified. Read ''help PsychHDR'' for list of valid commands.', cmd);
end

% Parse arguments provided by user-script in PsychImaging('AddTask','General','EnableHDR', ...)
% and turn them into a settings struct for internal use:
function hdrArgs = parseHDRArguments(hdrArguments)
    % hdrArguments{1} = Tasktype 'General'
    % hdrArguments{2} = Task 'EnableHDR'
    % hdrArguments{3} = unit of color input values.
    % hdrArguments{4} = hdrMode='Auto' aka 'HDR-10'.
    % hdrArguments{5} = extraRequirements='' by default.

    % Validate inputs:
    if ~strcmpi(hdrArguments{1}, 'General')
        error('PsychHDR-ERROR: Invalid task type ''%s'' provided for HDR operation.', hdrArguments{1});
    end

    if ~strcmpi(hdrArguments{2}, 'EnableHDR')
        error('PsychHDR-ERROR: Invalid task ''%s'' provided for HDR operation.', hdrArguments{2});
    end

    if isempty(hdrArguments{3})
        hdrArguments{3} = 'Nits';
    end

    switch lower(hdrArguments{3})
        case 'nits'
            hdrArgs.inputScalefactor = 1 / 10000;
            hdrArgs.contentSDRToHDRFactor = 80;
            hdrArgs.inputUnit = 'Nits';

        case '80nits'
            hdrArgs.inputScalefactor = 80 / 10000;
            hdrArgs.contentSDRToHDRFactor = 1;
            hdrArgs.inputUnit = '80Nits';

        otherwise
            error('PsychHDR-ERROR: Invalid input color value unit argument ''%s'' provided for HDR operation.', hdrArguments{3});
    end

    hdrArgs.unit = hdrArgs.inputUnit;

    if isempty(hdrArguments{4})
        hdrArguments{4} = 'Auto';
    end

    switch lower(hdrArguments{4})
        case 'auto'
            % Auto maps to only currently supported mode 'HDR-10' aka 1:
            hdrArgs.hdrMode = 1;

        case 'hdr10'
            % HDR-10: 10 bpc precision, BT-2020 color space, PQ EOTF:
            hdrArgs.hdrMode = 1;

        otherwise
            error('PsychHDR-ERROR: Invalid hdrMode ''%s'' provided for HDR operation.', hdrArguments{4});
    end

    % No dummy HDR mode by default:
    hdrArgs.dummy = 0;

    % Handle extraRequirements:
    if ~isempty(hdrArguments{5})
        if ~isempty(strfind(lower(hdrArguments{5}), 'dummy'))
            % Dummy mode requested, for minimal emulation on a SDR display:
            hdrArgs.hdrMode = 0;
            hdrArgs.dummy = 1;
        end
    end
end
