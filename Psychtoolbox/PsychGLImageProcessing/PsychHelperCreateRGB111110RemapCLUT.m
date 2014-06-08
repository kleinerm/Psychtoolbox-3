function remapCLUTId = PsychHelperCreateRGB111110RemapCLUT
% remapCLUTId = PsychHelperCreateRGB111110RemapCLUT;
%
% Helper function for Psychtoolbox imaging pipeline, called by
% PsychImaging(), not meant to be called by normal user code!
%
% Build a 3 rows by 2048 texels RGBA8 lookup texture for mapping HDR RGB
% pixel values in range 0-2047 (ie. 11 bpc resolution) to RGBA8 framebuffer
% pixels which will be suitable to drive a RGB11-11-10 framebuffer scanout
% engine (CRTC). If a graphics cards framebuffer scanout engine CRTC is
% configured for 11-11-10 ~ 11 bpc framebuffer mode, this will make sure that
% HDR pixel data is shown properly onscreen.
%
% This texture is used as CLUT texture for the
% RGBMultiLUTLookupCombine_FormattingShader.frag.txt in the final output
% formatting chain of the imaging pipeline.
%
% The expected pixel layout in 11bpc mode is: RGB111110, ie.:
% R11G11B10 -- That's how a 32 bit pixel is interpreted for video
% scanout. However the OpenGL system on all current OS with standard
% drivers can only output A8R8G8B8 formatted pixels. We solve this via the
% imaging pipeline: Screen() and OpenGL drawing ops go to the imaging
% pipelines virtual framebuffer with 16bpc float or 32 bpc float precision.
% At Flip time, our shader remaps that 0.0 - 1.0 values to the range 0-2047
% ie. approx. 11 bpc, then uses the CLUT texture created with this function to
% lookup corresponding ARGB8 color tuples for the framebuffer, combines
% them into a single ARGB8 tuple by addition (logical OR), then writes that
% ARGB tuple to the framebuffer. At display scanout time, the CRTC's will
% find 32 bit pixels properly formatted for RGB111110 scanout.
%
% History:
% 8-June-2014  Written - Derived from PsychHelperCreateARGB2101010RemapCLUT.m (MK).

% This routine assumes that a mogl GL context is properly set up:
global GL;

% Try to get LUT from cached .mat file -- faster:
cachedFile = [PsychtoolboxConfigDir 'rgb111110remaplut.mat'];

if exist(cachedFile, 'file')
    % Get cached LUT from file:
    load(cachedFile);
else
    % Build lookup table: 3 LUT's (Red,Green,Blue) with 2048 slots with 4
    % output color components RGBA8 per entry, ie., a 32 bpp value:
    clut = uint8(zeros(1, 3 * 2048 * 4));

    % Startindex for 1st LUT entry:
    j=1;

    % The order of channels vs. texture rows is backwards for some
    % reason, but this is the correct way for BGR10-11-11 scanout
    % with format code 7. Don't question it, bitshifting and swizzling
    % hurts the brain, just accept it.

    % First the 10 bpc, only 1024 levels BLUE LUT: Texrow 1
    for i=0:1023
        % Memory order is probably BGRA:

        % We only have 1024 intensity steps 'i', but because this
        % sub-LUT also has 2048 slots, we fill every two consecutive
        % slots with the same values, ie., do the fill process twice for
        % each value or step of i:
        for repeatsteap = 1:2
            % LSB 8 bits of blue into blue:
            clut(j)= bitand(i, bin2dec('11111111'));
            j=j+1;
            % MSB 2 bits of blue into 2 LSB of green:
            clut(j)= bitshift(bitand(i, bin2dec('1100000000')), -8);
            j=j+1;
            % Nothing in red:
            clut(j)= 0;
            j=j+1;
            % Nothing in alpha:
            clut(j)= 0;
            j=j+1;
        end
    end

    % Then the 11 bpc, 2048 levels GREEN LUT: Texrow 2
    for i=0:2047
        % Memory order is probably BGRA:

        % Nothing in blue
        clut(j)= 0;
        j=j+1;
        % LSB 6 bits of green into 6 MSB of green:
        clut(j)= bitshift(bitand(i, bin2dec('111111')), 2);
        j=j+1;
        % MSB 5 bits of green into 5 LSB of red:
        clut(j)= bitshift(bitand(i, bin2dec('11111000000')), -6);
        j=j+1;
        % Nothing in alpha:
        clut(j)= 0;
        j=j+1;
    end

    % First the 11 bpc, 2048 levels RED LUT: Texrow 3
    for i=0:2047
        % Memory order is probably BGRA:

        % Nothing in blue:
        clut(j)= 0;
        j=j+1;
        % Nothing in green:
        clut(j)= 0;
        j=j+1;
        % 3 LSB of red into 3 MSB of red:
        clut(j)= bitshift(bitand(i, bin2dec('111')), 5);
        j=j+1;
        % 8 MSB of red into 8 LSB of alpha:
        clut(j)= bitshift(bitand(i, bin2dec('11111111000')), -3);
        j=j+1;
    end

    % Cache computed LUT in users config dir:
    save(cachedFile, 'clut', '-mat', '-V6');
end

% Hostformat defines byte-order in host memory:
% This should work on any little-endian system (Windows/Linux On IntelPC,
% Linux on IntelMac, OS/X on IntelMac) - pretty much everything.
% On BigEndian systems (OS/X PowerPC) we may need to replace this by a
% GL.UNSIGNED_INT_8_8_8_8 instead to reverse byte order at conversion time.
hostformat = GL.UNSIGNED_INT_8_8_8_8_REV;

% Create and setup texture from 'clut':
remapCLUTId = glGenTextures(1);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, remapCLUTId);
glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, GL.RGBA, 2048, 3, 0, GL.BGRA, hostformat, clut);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

% Ready.
return;
