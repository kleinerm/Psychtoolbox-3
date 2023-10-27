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
% As of April 2021, these graphics cards would be suitable:
%
% - Modern AMD (RX 500 "Polaris" and later recommended) and NVidia (GeForce 1000
%   "Pascal" and later recommended) graphics cards under a Microsoft Windows-10
%   operating system, which is up to date for the year 2021.
%
% - Modern AMD graphics cards (like above) under modern GNU/Linux (Ubuntu 18.04.4-LTS
%   at a minimum (untested!), or better Ubuntu 20.04-LTS and later recommended), with
%   the AMD open-source Vulkan driver "amdvlk". Install driver release 2020-Q3.5 from
%   September 2020, which was tested, or any later versions. Note that release
%   2023-Q3.3 from September 2023 was the last release to support pre-Navi gpu's like
%   Polaris and Vega. Later versions only support AMD Navi and later with RDNA graphics
%   architecture.
%
%   The following webpage has amdvlk download and installation instructions:
%
%   https://github.com/GPUOpen-Drivers/AMDVLK/releases
%
% - Some Apple Mac computers, e.g., the MacBookPro 2017 15 inch Retina with AMD
%   graphics, under macOS 10.15.4 Catalina or later, do now have experimental and
%   limited HDR support. This has been tested with macOS 10.15.7 Catalina final,
%   on the MBP 2017 with AMD Radeon Pro 560 in a limited way on an external HDR-10
%   monitor, connected via USB-C to DisplayPort adapter. Precision of content
%   reproduction during leight testing was worse than on Linux and Windows. The
%   presentation timing was awful and unreliable, and performance was bad. Flexibility
%   and functionality was very limited in comparison to Windows-10, and even more so
%   compared to Linux. Querying HDR display properties from the HDR display is not
%   supported due to macOS limitations, and high performance HDR movie playback is
%   completely missing due to severe deficiencies of Apple's OpenGL implementation.
%   This uses the Apple Metal EDR "Extended dynamic range" support in macOS. Note
%   that Vulkan and HDR support on macOS is considered alpha quality at best, and
%   we do not provide any support for this feature. As always, if you care about
%   the quality of your results, use preferrably Linux, or Windows-10 instead.
%
%   You need at least MoltenVK version 1.1.4 and LunarG Vulkan SDK 1.2.182.0 from
%   5th July 2021 or later. MoltenVK v1.1.5 or later is recommended at this time.
%
%   Download link for the MoltenVK open-source "Vulkan on Metal" driver:
%
%   https://sdk.lunarg.com/sdk/download/latest/mac/vulkan-sdk.dmg
%   Overview on: https://vulkan.lunarg.com/sdk/home
%
%
% HDR functionality is demonstrated in multiple demos:
%
% SimpleHDRDemo.m as a simple starter for basic image display and rendering.
% HDRViewer.m as a more fancy static image viewer.
% HDRTest.m for testing HDR reproduction with a colorimeter supported by Psychtoolbox.
% MinimalisticOpenGLDemo.m with the optional 'hdr' parameter set to 1 for most basic OpenGL rendering in HDR.
% PlayMoviesDemo.m with the optional 'hdr' parameter set to 1 for playback of HDR movies.
%
% Useful helper functions beyond PsychImaging('AddTask', 'General', 'EnableHDR');
% for basic HDR setup and configuration, and PsychHDR() for tweaking, are
% the HDRRead() command for reading some HDR image file formats, e.g.,
% Radiance .hdr or OpenEXR .exr, ComputeHDRStaticMetadataType1ContentLightLevels()
% for computing HDR static metadata type one for an image or stack of
% images, and ConvertRGBSourceToRGBTargetColorSpace() for converting images
% from a source color space / gamut to a destination color space / gamut.
%
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
persistent targetUUIDs;
persistent oldHDRMeta;
persistent oldHDRProperties;

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

    if (~isempty(varargin)) && ~isempty(varargin{1})
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
        lasterror('reset'); %#ok<*LERR>
        varargout{1} = 0;
    end

    return;
end

% hdrImagingModeFlags = PsychHDR('GetClientImagingParameters', hdrArguments);
if strcmpi(cmd, 'GetClientImagingParameters')
    % Parse caller provided EnableHDR task parameters into an easily usable
    % hdrArgs settings struct, while validating user input:
    hdrArgs = parseHDRArguments(varargin{1}); %#ok<NASGU>

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
    win = varargin{1}; %#ok<NASGU>

    % Parse caller provided EnableHDR task parameters into an easily usable
    % hdrArgs settings struct, while validating user input:
    hdrArgs = parseHDRArguments(varargin{2});

    % useVulkan = 1, as we currently only do HDR via Vulkan/WSI:
    varargout{1} = 1;

    % vulkanHDRMode:
    varargout{2} = hdrArgs.hdrMode;

    % vulkanColorPrecision = 0 -- Derive from vulkanHDRMode by default:
    varargout{3} = 0;

    % vulkanColorSpace:
    varargout{4} = hdrArgs.colorSpace;

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

    % Special static HDR stereo hack on Linux/X11 enabled?
    if hdrArgs.dummy ~= 2
        % No, standard case: Retrieve actual parameters of Vulkan window:
        hdrWindowProps = PsychVulkan('GetHDRProperties', win);
    else
        % Yes: We don't have an actual Vulkan window available at this point,
        % only the regular Screen() onscreen window 'win', spanning all displays
        % of a HDR setup. Therefore we can not query hdrWindowProps and need to
        % make them up in a way compatible with this special purpose hack.
        % Assign something that triggers the 'otherwise' case below:
        hdrWindowProps.ColorSpace = 1;
    end

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

            % Select scalefactor from user framebuffer to shader input:
            % Input scaling from input unit to 0 - 1 range, where 1 = 10000 nits:
            scalefactor = hdrArgs.inputScalefactor;

            % Set scaling factors for mapping SDR or HDR image and movie content into our HDR linear color space,
            % using the correct unit of luminance. Used by movie playback and Screen('MakeTexture') among others:
            Screen('HookFunction', win, 'SetHDRScalingFactors', [], hdrArgs.contentSDRToHDRFactor, 1 / scalefactor);

            % Set color gamut of HDR color space:
            Screen('HookFunction', win, 'WindowColorGamut', [], hdrArgs.colorGamut);

            % Load PQ shader:
            oetfshader = LoadGLSLProgramFromFiles('HDR10-PQ_Shader', [], icmshader);

            % Define mapping from BT-2020 colorspace to output colorspace:
            switch (hdrWindowProps.ColorSpace)
                case 1000104002 % VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT
                    % scRGB colorspace with linear encoding 0-10000 nits ==> 0-125:
                    eotfName = 'scRGB-Linear';
                    doPQEncode = 0;

                    % macOS as usual needs special treatment...
                    if IsOSX
                        % As of MoltenVK 1.1.3, HDRMetaData setup via vkSetHDRMetadataEXT()
                        % hard-codes a Metal surface CAEDRMetadata opticalOutputScale
                        % factor of 1.0 nits, which means that provided pixel
                        % color values are supposed to be interpreted as
                        % unit of nits. Therefore we must scale by 10000
                        % for a mapping from [0; 1] input to [0; 10000 nits]:
                        scalefactor = scalefactor * 10000;
                    else
                        % Linear encoding is simply multiplication by 125:
                        scalefactor = scalefactor * 125;
                    end

                    % Need CSC from BT-2020 to scRGB:
                    doCSC = 1;

                    % scRGB color gamut with D65 white point and identical color gamut to sRGB and BT-709:
                    gamutRec709 = [[0.64 ; 0.33], [0.30 ; 0.60], [0.15 ; 0.06], [0.31271 ; 0.32902]];
                    MCSC = ConvertRGBSourceToRGBTargetColorSpace(hdrArgs.colorGamut, gamutRec709);

                case 1000213000 % VK_COLOR_SPACE_DISPLAY_NATIVE_AMD
                    if hdrWindowProps.ColorFormat == 97 % == VK_FORMAT_R16G16B16A16_SFLOAT
                        % FS2 scRGB mode: Implemented, but not tested/validated, possibly incomplete!
                        %
                        % scRGB colorspace with linear encoding 0-10000
                        % nits ==> 0-125, more specifically from display
                        % minimum luminance / 80 to display maximum
                        % luminance / 80, but for a max 10k nits display
                        % that would translate to 0 - 125:
                        eotfName = 'FS2-scRGB-Linear';
                        doPQEncode = 0;

                        % Linear encoding is simply multiplication by 125:
                        scalefactor = scalefactor * 125;

                        % Need CSC from BT-2020 to scRGB:
                        doCSC = 1;

                        % scRGB color gamut with D65 white point and identical color gamut to sRGB and BT-709:
                        gamutRec709 = [[0.64 ; 0.33], [0.30 ; 0.60], [0.15 ; 0.06], [0.31271 ; 0.32902]];
                        MCSC = ConvertRGBSourceToRGBTargetColorSpace(hdrArgs.colorGamut, gamutRec709);
                    else
                        % FS2 Gamma 2.2 mode: TODO!
                        error('PsychHDR: FreeSync2 Gamma 2.2 HDR mode is not yet implemented!');
                    end

                    warning('PsychHDR: AMD FreeSync2 HDR modes are incomplete, not officially supported, not validated, and possibly faulty!');

                otherwise % VK_COLOR_SPACE_HDR10_ST2084_EXT
                    % Standard HDR-10 BT-2020 colorspace with PQ encoding:
                    eotfName = 'BT-2020-PQ';
                    doPQEncode = 1;

                    % From BT-2020 to BT-2020, therefore CSC not needed:
                    doCSC = 0;
                    MCSC = [[1 0 0]; [0 1 0]; [0 0 1]];
            end

            if verbosity >= 3
                if hdrArgs.dummy ~= 1
                    % The real thing:
                    fprintf('PsychHDR-INFO: HDR-10 mode activated. BT-2020 input window color space, pixel color unit is %s. Output uses %s EOTF.\n', hdrArgs.inputUnit, eotfName);
                else
                    % Only minimal emulation:
                    fprintf('PsychHDR-INFO: HDR-10 mode EMULATION ON SDR DISPLAY activated. BT-2020 color space, PQ EOTF. Unit is %s.\n', hdrArgs.inputUnit);
                    fprintf('PsychHDR-INFO: This is only a most bare-bones emulation. VISUAL STIMULI WILL DISPLAY WRONG!\n');
                end
            end

            % Set it up - Assign texture image unit 0 and input values
            % scalefactor:
            glUseProgram(oetfshader);
            glUniform1i(glGetUniformLocation(oetfshader, 'Image'), 0);
            glUniform1f(glGetUniformLocation(oetfshader, 'Prescale'), scalefactor);
            glUniform1i(glGetUniformLocation(oetfshader, 'doPQEncode'), doPQEncode);
            glUniform1i(glGetUniformLocation(oetfshader, 'doCSC'), doCSC);
            glUniformMatrix3fv(glGetUniformLocation(oetfshader, 'MCSC'), 1, 0, MCSC);
            glUseProgram(0);

            % Assign shader name: Add name of color correction shader:
            oetfshaderstring = sprintf('HDR-OETF-%s-Formatter: %s', eotfName, icmstring);

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

    % Check if this window is associated with a Vulkan/WSI / Vulkan window:
    winfo = Screen('GetWindowInfo', window, 7);
    if ~bitand(winfo.ImagingMode, kPsychNeedFinalizedFBOSinks)
        % Nope: Must be part of the static HDR stereo hack.

        % Return retrieved old HDR metadata settings of 1st HDR monitor:
        if length(oldHDRMeta) >= window
            varargout{1} = oldHDRMeta{window};
        else
            varargout{1} = [];
        end

        % These are the only settings we support for the static HDR Linux/X11 hack:
        flags = 0;
        gpuIndex = 0;
        vulkanHDRMode = 1;

        % Perform HDR update hack:
        PsychHDR('ExecuteStaticHDRHack', window, 2, vulkanHDRMode, gpuIndex, flags, meta);

        % Assign new settings as future old cached settings:
        oldHDRMeta{window} = meta;

        return;
    end

    [ varargout{1:nargout} ] = PsychVulkan(cmd, window, meta.MetadataType, meta.MaxFrameAverageLightLevel, meta.MaxContentLightLevel, meta.MinLuminance, meta.MaxLuminance, meta.ColorGamut);

    return;
end

% PsychHDR('ExecuteStaticHDRHack', win, enable, vulkanHDRMode, gpuIndex, flags);
if strcmpi(cmd, 'ExecuteStaticHDRHack')
    win = varargin{1};
    enable = varargin{2};
    vulkanHDRMode = varargin{3};
    gpuIndex = varargin{4};
    flags = bitor(varargin{5}, 1); % Must have no OpenGL-Vulkan interop.
    hdrMetadata = varargin{6}; % Struct with HDR metadata to use.

    % Get screenId of the X-Screen on which our onscreen window displays:
    screenId = Screen('WindowScreenNumber', win);

    if enable
        % Get the UUID of the Vulkan device that is compatible with our associated
        % OpenGL renderer/gpu. Compatible means: Can by used for OpenGL-Vulkan interop:
        winfo = Screen('GetWindowInfo', win);
        if ~isempty(winfo.GLDeviceUUID)
            targetUUID = winfo.GLDeviceUUID;
        else
            % None provided, because the OpenGL implementation does not support
            % OpenGL-Vulkan interop. Assign empty id for most basic testing:
            targetUUID = zeros(1, 16, 'uint8');

            % Lacking any better way to choose the gpu, assume gpuIndex 1 is the
            % right choice: TODO: Could go for GL_VENDOR or such for simple cases...
            gpuIndex = 1;
        end

        % Store this targetUUID in an internal persistent per-window variable:
        targetUUIDs{win} = targetUUID;

        % We want an identity hardware gamma lut in HDR, but at maximum lut precision,
        % so output does not get truncated to 8 bpc. Therefore we can't use
        % LoadIdentityClut() which is aimed at 8 bpc identity pixel passthrough.
        % This is only done on first-time enable hack:
        if enable == 1
            % Backup old lut for this screen:
            BackupCluts(screenId);

            % Upload a perfectly linear lut for the given gpu:
            [~, ~, reallutsize] = Screen('ReadNormalizedGammaTable', win);

            % AMD gpu under Linux?
            if IsLinux && strcmp(winfo.DisplayCoreId, 'AMD')
                % Use special identity gamma table (like in LoadIdentityClut()) that
                % is known and verified to get recognized by amdgpu-kms DC and trigger
                % gamma table hardware bypass mode in hardware:
                identityLUT = (linspace(0, 1, 256)' * ones(1, 3));
            else
                % Other gpu + driver + os combo: Standard identity lut:
                identityLUT = repmat(linspace(0, 1, reallutsize)', 1, 3);
            end

            Screen('LoadNormalizedGammaTable', win, identityLUT, 0);

            if verbosity >= 3
                fprintf('PsychHDR-INFO: Loaded identity gamma table into X-Screen %i for HDR.\n', screenId);
            end
        end
    else
        % Disable: Can not do a 'GetWindowInfo' query for targetUUID, because we
        % are executing from within the Screen('Close', win) path, where calls to
        % that function are unsafe (crash!). Retrieve the targetUUID that we cached
        % during the previous enable call:
        targetUUID = targetUUIDs{win};
        targetUUIDs{win} = [];
    end

    % Get video output properties of primary display output monitor on the X-Screen:
    output = Screen('ConfigureDisplay', 'Scanout', screenId, 0);

    % Rect needs to define the video resolution of the selected video mode, ie.,
    % wanted width x height in pixels. It is static across all participating
    % monitors, as either there is only one such monitor, or in a stereo dual-
    % display setup or similar, we need identical video modes on all participating
    % displays for synchronized video refresh cycles:
    rect = [0, 0, output.width, output.height];

    % Target video refreshHz is also identical for all participating displays:
    refreshHz = output.hz;

    % Build list of RandR outputHandle's for all outputs in a single/dual-display
    % or multi-display stimulation setup for stereo or surround style stimulation:
    outputHandle = uint64([]);
    for i = 0:(Screen('ConfigureDisplay', 'NumberOutputs', screenId) - 1)
        output = Screen('ConfigureDisplay', 'Scanout', screenId, i);
        outputHandle(end+1) = uint64(output.outputHandle);
    end

    if enable
        if enable == 1
            cmdString = sprintf('PsychHDR(''ExecuteStaticHDRHack'', %i, 0, %i, %i, %i, [])', win, vulkanHDRMode, gpuIndex, flags);
            Screen('Hookfunction', win, 'PrependMFunction', 'CloseOnscreenWindowPostGLShutdown', 'Vulkan HDR hack cleanup', cmdString);
            Screen('Hookfunction', win, 'Enable', 'CloseOnscreenWindowPostGLShutdown');
            cmdString = 'octave-cli --no-gui --no-history --silent --eval ''PsychHDR("DoExecuteStaticHDRHack", 1)''';
        else
            cmdString = 'octave-cli --no-gui --no-history --silent --eval ''PsychHDR("DoExecuteStaticHDRHack", 2)''';
        end
    else
        cmdString = 'octave-cli --no-gui --no-history --silent --eval ''PsychHDR("DoExecuteStaticHDRHack", 0)''';
    end

    if verbosity > 2
        switch(enable)
        case 0,
            fprintf('PsychHDR-INFO: Executing Vulkan one-time HDR disable hack in helper process - Engage!\n');
        case 1,
            fprintf('PsychHDR-INFO: Executing Vulkan one-time HDR enable hack in helper process - Today is a good day to die! Engage!\n');
        case 2,
            fprintf('PsychHDR-INFO: Executing Vulkan one-time HDR static metadata setup hack in helper process - Today is a good day to die! Engage!\n');
        end
    end

    for outputId = outputHandle
        % Store all relevant variables with parameters into a file, so our helper
        % process can read them from there. This format should work for both Octave
        % and Matlab, also across Matlab and Octave:
        save([PsychtoolboxConfigDir 'PsychHDRIPC.mat'], '-mat', '-V6');

        % Execute helper process:
        cmdString = [cmdString ' 2>&1'];
        [rc, msg] = system(cmdString);
        if ~ismember(rc, [0, 137])
            % Trouble!
            fprintf('PsychHDR-ERROR: Switch for outputId %i - Failed! Error output from helper process [rc=%i]:\n\n', outputId, rc);
            disp(msg);
            error('PsychHDR-ERROR: Vulkan en-/disable sequence for static HDR display hack failed! See error above.');
        end

        if verbosity > 4
            fprintf('PsychHDR-DEBUG: Switch for outputId %i - Killer trick success! Debug output from helper process:\n\n', outputId);
            disp(msg);
            fprintf('PsychHDR-DEBUG: Done. Success!\n');
        elseif verbosity > 3
            if enable
                fprintf('PsychHDR-INFO: For outputId %i - Vulkan one-time HDR setup killer trick - Success!\n', outputId);
            else
                fprintf('PsychHDR-INFO: For outputId %i - Vulkan HDR teardown hack - Success!\n', outputId);
            end
        end

        % Retrieve HDR properties and metadata settings provided by helper process,
        % but only for first output, as the working assumption for multi-display is
        % that all HDR monitors are identical models with identical properties and
        % settings:
        if enable && (outputId == outputHandle(1))
            load([PsychtoolboxConfigDir 'PsychHDRIPC.mat'], 'oldHdrMetadata', 'hdrProperties');
            oldHDRMeta{win} = oldHdrMetadata;
            oldHDRProperties{win} = hdrProperties;
        end
    end

    % Give X-Server some time to settle, as some of this is a bit racy:
    WaitSecs(0.25);

    return;
end

if strcmpi(cmd, 'DoExecuteStaticHDRHack')
    % Load all relevant variables with parameters into a file, so our helper
    % process can read them from there. This format should work for both Octave
    % and Matlab, also across Matlab and Octave:
    load([PsychtoolboxConfigDir 'PsychHDRIPC.mat']);

    % 'enable' encodes type of request: 1/2 = Enable, 0 = Disable HDR.

    PsychVulkanCore('Verbosity', verbosity);

    % Open a Vulkan fullscreen window on target output 'outputId', with suitable
    % hdrMode enabled, on the proper gpu gpuIndex / targetUUID and X-Screen screenId.
    %
    % This should trigger direct display mode and sending of HDR metadata to switch
    % HDR monitors into HDR-10 mode:
    vwin = PsychVulkanCore('OpenWindow', gpuIndex, targetUUID, 1, screenId, rect, outputId, vulkanHDRMode, 1, refreshHz, 0, 0, flags);

    % Disable of HDR mode requested?
    if ~enable
        % Yes. Simply close the HDR window after opening it in HDR mode. This will
        % not only close the window, but also send the HDR disable command to the
        % Linux kernel, given that we just enabled HDR during the 'OpenWindow':
        PsychVulkanCore('CloseWindow', vwin);

        % We are done and can return control to the calling process, which will
        % then exit cleanly:
        return;
    end

    % Enable of HDR mode or change of HDR static metadata requested:

    % Custom static HDR metadata provided by caller?
    if ~isempty(hdrMetadata)
        % Yes: Set custom caller provided static HDR metadata for this session:
        oldHdrMetadata = PsychVulkanCore('HDRMetadata', vwin, hdrMetadata.MetadataType, hdrMetadata.MaxFrameAverageLightLevel, hdrMetadata.MaxContentLightLevel, hdrMetadata.MinLuminance, hdrMetadata.MaxLuminance, hdrMetadata.ColorGamut);

        % Trigger a single present to latch the new HDR metadata to the HDR monitor:
        PsychVulkanCore('Present', vwin, 0, 1);
    else
        oldHdrMetadata = PsychVulkanCore('HDRMetadata', vwin);
    end

    % Return old HDR metadata and properties:
    hdrProperties = PsychVulkanCore('GetHDRProperties', vwin);
    save([PsychtoolboxConfigDir 'PsychHDRIPC.mat'], '-mat', '-V6');

    % End of enable sequence, now we kill ourselves: Today is a good day to die!
    % This will kill the Vulkan driver and hosting Octave/Matlab process without
    % closing the Vulkan window. As a result, the Linux kernel will clean up after
    % our dead process, releasing all resources and also releasing the HDR monitor/
    % RandR output back to the X-Server and thereby to the regular Screen() fullscreen
    % onscreen window that we want to use for purely OpenGL driven HDR stimulus
    % presentation without any further involvement of Vulkan.
    % We do the kill, because if our driver/process gets killed, the one thing
    % the Linux kernel does not do (as of Linux 5.8 at least) is disable HDR
    % metadata transmission to the HDR monitor. So the HDR monitor continues to
    % receive our static HDR metadata and stays in HDR-10 mode with the statically
    % assigned HDR properties, ready to receive OpenGL rendered and displayed HDR
    % PQ encoded visual stimuli for display:
    kill(getpid, 9);

    % Never reached:
    return;
end

% Dispatch subfunctions with "HDR" in their name to PsychVulkan(), which may
% hand them off to PsychVulkanCore():
if ~isempty(strfind(lower(cmd), 'hdr')) %#ok<*STREMP>
    if ~isempty(varargin)
        % Check if 1st arg is a window and if it is associated with a Vulkan/WSI / Vulkan window:
        if ~isempty(varargin{1}) && isscalar(varargin{1}) && isreal(varargin{1}) && (Screen('WindowKind', varargin{1}) == 1)
            winfo = Screen('GetWindowInfo', varargin{1}, 7);
            if ~bitand(winfo.ImagingMode, kPsychNeedFinalizedFBOSinks)
                % Nope: Windows must be part of the static HDR stereo hack. Can we handle it?
                if strcmpi(cmd, 'HDRMetadata')
                    % Return cached HDR metadata from last enable/update hack:
                    meta = oldHDRMeta{varargin{1}};
                    varargout{1} = meta;

                    % Optionally assign new HDR metadata by parsing args into struct,
                    % then using that setup code:
                    if length(varargin) >= 2
                        meta.MetadataType = varargin{2};

                        if length(varargin) >= 3
                            meta.MaxFrameAverageLightLevel = varargin{3};
                        end

                        if length(varargin) >= 4
                            meta.MaxContentLightLevel = varargin{4};
                        end

                        if length(varargin) >= 5
                            meta.MinLuminance = varargin{5};
                        end

                        if length(varargin) >= 6
                            meta.MaxLuminance = varargin{6};
                        end

                        if length(varargin) >= 7
                            meta.ColorGamut = varargin{7};
                        end

                        PsychHDR('HDRMetadata', varargin{1}, meta);
                    end

                    return;
                end

                if strcmpi(cmd, 'GetHDRProperties')
                    % Returned cached HDR properties from last enable hack:
                    varargout{1} = oldHDRProperties{varargin{1}};
                    return;
                end

                if strcmpi(cmd, 'HDRLocalDimming')
                    % Not supported on Linux atm., so always report disabled:
                    varargout{1} = 0;

                    % Reject any setup requests:
                    if (length(varargin) > 1) && ~isempty(varargin{2})
                        error('PsychHDR(''HDRLocalDimming'') control is not supported on Linux in OpenGL HDR hack mode!');
                    end

                    return;
                end

                % This call is unsupported:
                warning(sprintf('PsychHDR(''%s'') call unsupported in static HDR OpenGL Linux hack mode. Ignored!', cmd));

                varargout{1} = [];
                return;
            end
        end

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

    % Map hdrMode to default color gamut for that mode:
    switch (hdrArgs.hdrMode)
        case 1
            % HDR-10, BT-2100 container, ie. BT-2020 aka ITU Rec.2020 color space with D65 white point:
            hdrArgs.colorGamut = [[0.708 ; 0.292], [0.170 ; 0.797], [0.131 ; 0.046], [0.31271 ; 0.32902]];
            hdrArgs.colorSpace = 0; % Auto-Select Vulkan colorspace based on hdrMode.

            %hdrArgs.colorSpace = 1000213000; % This would request VK_COLOR_SPACE_DISPLAY_NATIVE_AMD for Freesync2 HDR. Disabled atm., because this is neither complete nor validated at all!

        otherwise
            error('PsychHDR-ERROR: Default color gamut for hdrMode %i unknown for HDR operation! PsychHDR() implementation bug?!?', hdrArgs.hdrMode);
    end

    % No dummy HDR mode by default:
    hdrArgs.dummy = 0;

    % Handle extraRequirements:
    if ~isempty(hdrArguments{5})
        if ~isempty(strfind(lower(hdrArguments{5}), 'statichdrhack'))
            % Static HDR stereo hack on Linux/X11 requested, for driving the HDR display via purely OpenGL:
            hdrArgs.dummy = 2;
        end

        if ~isempty(strfind(lower(hdrArguments{5}), 'dummy'))
            % Dummy mode requested, for minimal emulation on a SDR display:
            hdrArgs.hdrMode = 0;
            if hdrArgs.dummy == 0
                hdrArgs.dummy = 1;
            end
        end
    end
end
