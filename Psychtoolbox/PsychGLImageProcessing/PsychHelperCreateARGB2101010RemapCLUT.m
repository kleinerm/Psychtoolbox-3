function remapCLUTId = PsychHelperCreateARGB2101010RemapCLUT
% remapCLUTId = PsychHelperCreateARGB2101010RemapCLUT;
%
% Helper function for Psychtoolbox imaging pipeline, called by
% PsychImaging(), not meant to be called by normal user code!
%
% Build a 3 rows by 1024 texels RGBA8 lookup texture for mapping HDR RGB
% pixel values in range 0-1023 (ie. 10 bpc resolution) to RGBA8 framebuffer
% pixels which will be suitable to drive a ARGB2101010 framebuffer scanout
% engine (CRTC). If a graphics cards framebuffer scanout engine CRTC is
% configured for 2101010 10 bpc framebuffer mode, this will make sure that
% HDR pixel data is shown properly onscreen.
%
% This texture is used as CLUT texture for the
% RGBMultiLUTLookupCombine_FormattingShader.frag.txt in the final output
% formatting chain of the imaging pipeline.
%
% The expected pixel layout in 10bpc mode is: ARGB2101010, ie.:
% A2R10G10B10 -- That's how a 32 bit pixel is interpreted for video
% scanout. However the OpenGL system on all current OS with standard
% drivers can only output A8R8G8B8 formatted pixels. We solve this via the
% imaging pipeline: Screen() and OpenGL drawing ops go to the imaging
% pipelines virtual framebuffer with 16bpc float or 32 bpc float precision.
% At Flip time, our shader remaps that 0.0 - 1.0 values to the range 0-1023
% ie. 10 bpc, then uses the CLUT texture created with this function to
% lookup corresponding ARGB8 color tuples for the framebuffer, combines
% them into a single ARGB8 tuple by addition (logical OR), then writes that
% ARGB tuple to the framebuffer. At display scanout time, the CRTC's will
% find 32 bit pixels properly formatted for ARGB2101010 scanout.
%
% History:
% 01/13/08  Written (MK).
% 08/29/11  Add caching for LUT in mat file to speed this up a bit (MK).

% This routine assumes that a mogl GL context is properly set up:
global GL;

% Try to get LUT from cached .mat file -- faster:
cachedFile = [PsychtoolboxConfigDir 'argb2101010remaplut.mat'];

if exist(cachedFile, 'file')
    % Get cached LUT from file:
    load(cachedFile);
else
    % Build lookup table: 3 LUT's (Red,Green,Blue) with 1024 slots with 4
    % output color components RGBA8 per entry, ie., a 32 bpp value:
    clut = uint8(zeros(1, 3 * 1024 * 4));

    % Startindex for 1st LUT entry:
    j=1;

    % First the RED LUT: Texrow 1
    for i=0:1023
        % Memory order is ARGB:

        % Nothing in blue:
        clut(j)= 0;
        j=j+1;
        % Nothing in green:
        clut(j)= 0;
        j=j+1;
        % 4 LSB of red into 4 MSB of red:
        clut(j)= bitshift(bitand(i, bin2dec('1111')), 4);
        j=j+1;
        % 6 MSB of red into 6 LSB of alpha:
        clut(j)= bitshift(bitand(i, bin2dec('1111110000')), -4);
        j=j+1;
        % 2 MSB of alpha are left to a constant zero. They would encode the 2
        % bit alpha channel, but that is unused in our configuration. Alpha is
        % handled at render time by the high-res drawbuffer and earlier part of
        % imaging pipeline...
    end

    % Then the GREEN LUT: Texrow 2
    for i=0:1023
        % Memory order is ARGB:

        % Nothing in blue
        clut(j)= 0;
        j=j+1;
        % LSB 6 bits of green into 6 MSB of green:
        clut(j)= bitshift(bitand(i, bin2dec('111111')), 2);
        j=j+1;
        % MSB 4 bits of green into 4 LSB of red:
        clut(j)= bitshift(bitand(i, bin2dec('1111000000')), -6);
        j=j+1;
        % Nothing in alpha:
        clut(j)= 0;
        j=j+1;
    end

    % Finally the BLUE LUT: Texrow 3
    for i=0:1023
        % Memory order is ARGB:

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
glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, GL.RGBA, 1024, 3, 0, GL.BGRA, hostformat, clut);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

% Ready.
return;
