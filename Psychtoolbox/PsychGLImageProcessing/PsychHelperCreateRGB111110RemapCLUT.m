function remapCLUTId = PsychHelperCreateRGB111110RemapCLUT
% remapCLUTId = PsychHelperCreateRGB111110RemapCLUT;
%
% Helper function for Psychtoolbox imaging pipeline, called by
% PsychImaging(), not meant to be called by normal user code!
%
% Build a 3 rows by 2048 texels RGBA8 lookup texture for mapping HDR RGB
% pixel values in range 0-2047 (ie. 11 bpc resolution) to RGBA8 framebuffer
% pixels which will be suitable to drive a BGR10-10-11 framebuffer scanout
% engine (CRTC). If a graphics cards framebuffer scanout engine CRTC is
% configured for 10-11-11 ~ 11 bpc framebuffer mode, this will make sure that
% HDR pixel data is shown properly onscreen.
%
% This texture is used as CLUT texture for the
% RGBMultiLUTLookupCombine_FormattingShader.frag.txt in the final output
% formatting chain of the imaging pipeline.
%
% The expected pixel layout in 11bpc mode is: BGR101111, ie.:
% R11 G11 B10 -- That's how a 32 bit pixel is interpreted for video
% scanout. However the OpenGL system on all current OS with standard
% drivers can only output A8R8G8B8 formatted pixels. We solve this via the
% imaging pipeline: Screen() and OpenGL drawing ops go to the imaging
% pipelines virtual framebuffer with 16bpc float or 32 bpc float precision.
% At Flip time, our shader remaps that 0.0 - 1.0 values to the range 0-2047
% ie. approx. 11 bpc, then uses the CLUT texture created with this function to
% lookup corresponding ARGB8 color tuples for the framebuffer, combines
% them into a single ARGB8 tuple by addition (logical OR), then writes that
% ARGB tuple to the framebuffer. At display scanout time, the CRTC's will
% find 32 bit pixels properly formatted for BGR101111 scanout.
%
% This requires a suitable graphics cards with an at least 11 bpc capable
% display pipeline. This is the case for recent AMD gpu's of the "Sea Islands"
% and "Volcanic Islands" family with a DCE-8 or later display engine. Those are
% said to have full 12 bpc pipelines.
%
% History:
%  8-Jun-2014  Written - Derived from PsychHelperCreateARGB2101010RemapCLUT.m (MK).
% 25-May-2016  Fixed up for DCE-10 "Volcanic Islands" Radeon R9 380 Tonga Pro (MK).
%              Turns out they need very different formatting and our original
%              implementation on non-12 bpc capable gpu's caused wrong test results.

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

    % This is the correct input to output color channels/bits mapping for the
    % BGR10-11-11 scanout with 32bpp format code 7 as used by the GPU.
    %
    %  Input:  -> Output
    %    B2:9  -> A0:7
    %    B0:1  -> R6:7
    %    G5:10 -> R0:5
    %    G0:4  -> G3:7
    %    R8:10 -> G0:2
    %    R0:7  -> B0:7

    % First the 11 bpc, 2048 levels RED LUT: Texrow 1
    for i=0:2047
        % Memory order for texture creation is RGBA:

        % Nothing in R:
        clut(j)= 0;
        j=j+1;
        % R8:10 -> G0:2:
        clut(j)= bitshift(bitand(i, bin2dec('11100000000')), -8);
        j=j+1;
        % R0:7  -> B0:7
        clut(j)= bitshift(bitand(i, bin2dec('00011111111')), +0);
        j=j+1;
        % Nothing in A:
        clut(j)= 0;
        j=j+1;
    end

    % Then the 11 bpc, 2048 levels GREEN LUT: Texrow 2
    for i=0:2047
        % Memory order for texture creation is RGBA:

        % G5:10 -> R0:5:
        clut(j)= bitshift(bitand(i, bin2dec('11111100000')), -5);
        j=j+1;
        % G0:4  -> G3:7:
        clut(j)= bitshift(bitand(i, bin2dec('00000011111')), +3);
        j=j+1;
        % Nothing in B:
        clut(j)= 0;
        j=j+1;
        % Nothing in A:
        clut(j)= 0;
        j=j+1;
    end

    % Last the only 10 bpc, only 1024 levels BLUE LUT: Texrow 3
    for i=0:1023
        % Memory order for texture creation is RGBA:

        % We only have 1024 intensity steps 'i', but because this
        % sub-LUT also has 2048 slots, we fill every two consecutive
        % slots with the same values, ie., do the fill process twice for
        % each value or step of i:
        for repeatsteap = 1:2
            % B0:1 - > R6:7
            clut(j)= bitshift(bitand(i, bin2dec('0000000011')), +6);
            j=j+1;
            % Nothing in G:
            clut(j)= 0;
            j=j+1;
            % Nothing in B:
            clut(j)= 0;
            j=j+1;
            % B2:9  -> A0:7:
            clut(j)= bitshift(bitand(i, bin2dec('1111111100')), -2);
            j=j+1;
        end
    end

    % Cache computed LUT in users config dir:
    save(cachedFile, 'clut', '-mat', '-V6');
end

% Hostformat defines byte-order in host memory:
% At increasing memory locations for each texel first R, then G, then B, then A.
hostformat = GL.UNSIGNED_BYTE;

% Create and setup texture from 'clut':
remapCLUTId = glGenTextures(1);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, remapCLUTId);
glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, GL.RGBA8, 2048, 3, 0, GL.RGBA, hostformat, clut);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

% Ready.
return;
