function remapCLUTId = PsychHelperCreateRemapCLUT(cmd, arg1, arg2)
% remapCLUTId = PsychHelperCreateRemapCLUT(cmd, arg);
%
% Helper function for Psychtoolbox imaging pipeline, called by
% PsychImaging(), not meant to be called by normal user code!
%
% If 'cmd' command code is zero, then:
%
% Build a 3 rows by arg1 texels RGBA8 lookup texture for mapping of RGB
% pixel values in range 0 to (arg-1) to RGBA8 framebuffer pixels. This
% texture is used as CLUT texture for the
% RGBMultiLUTLookupCombine_FormattingShader.frag.txt in the image
% processing chains of the imaging pipeline. If arg2 is set to 1 instead of
% zero, a 32 bpc float texture is used instead of 8 bpc integer.
%
% If 'cmd' command code is 1, then the clut texture with id arg1 is updated
% with the content of clut table arg2.
%

% History:
% 03.04.2011 Written (MK).

% This routine assumes that a mogl GL context is properly set up:
global GL;

% Keep our working lut buffers persistent, for speedup during runtime:
persistent rlut;
persistent glut;
persistent blut;

if cmd == 1
    % New gamma table received from Screen()'s 'LoadNormalizedGammatable'
    % command. Need to update our clut texture, so this applies during next
    % execution of Screen('Flip'):
    
    % Update lookup table: 3 LUT's (Red,Green,Blue) with nslots slots with 4
    % output color components RGBA8 per entry, ie., a 32 bpp value:
    nslots = size(arg2, 1);

    % Update red, green and blue luts. Assignment needs to preserve
    % single() format aka GL.FLOAT:
    rlut(1,:) = single(arg2(:,1));
    glut(2,:) = single(arg2(:,2));
    blut(3,:) = single(arg2(:,3));
    
    % Build final clut memory buffer for texture update:
    clut = [ rlut(:) ; glut(:) ; blut(:) ]; 

    % Bind clut texture for update:
    remapCLUTId = arg1;
    glActiveTexture(GL.TEXTURE1_ARB);
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, remapCLUTId);
    glTexSubImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, 0, 0, nslots, 3, GL.RGBA, GL.FLOAT, clut);
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);
    glActiveTexture(GL.TEXTURE0_ARB);

    % Ready.
    return;
end

if cmd == 0
    % Build initial clut texture during init call from PsychImaging():
    
    nslots = arg1;
    
    % Preinit our lut arrays, so we save (re-)allocation time in the
    % realtime path for command code 1:
    rlut = single(zeros(4, nslots));
    glut = single(zeros(4, nslots));
    blut = single(zeros(4, nslots));

    % High precision > 8 bpc clut requested?
    internalFormat = GL.RGBA;
    if arg2 > 0
        % Yes. High precision supported?
        win = Screen('GetOpenGLDrawMode');
        winfo = Screen('GetWindowInfo', win);
        if winfo.GLSupportsTexturesUpToBpc >= 32
            % Yes: Use full 32 bits single precision rgba:
            internalFormat = GL.RGBA_FLOAT32_APPLE;
            fprintf('EnableCLUTMapping: Using a 32 bit float CLUT -> 23 bits effective linear mapping precision per color channel.\n');
        else
            % No: Stick to 8 bpc and warn user:
            fprintf('EnableCLUTMapping: ERROR! High precision CLUT requested, but hardware does not support this!\n');
            fprintf('EnableCLUTMapping: Aborting. Use different hardware or fix your scripts PsychImaging(...''EnableCLUTMapping'' ...); function.');
            error('EnableCLUTMapping: Aborting. Use different hardware or fix your scripts PsychImaging(...''EnableCLUTMapping'' ...); function.');
        end
    end
    
    % Create and setup texture with arg1 slots:
    remapCLUTId = glGenTextures(1);
    glActiveTexture(GL.TEXTURE1_ARB);
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, remapCLUTId);
    glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, internalFormat, nslots, 3, 0, GL.RGBA, GL.FLOAT, single(zeros(nslots, 3)));
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);
    glActiveTexture(GL.TEXTURE0_ARB);
    return;
end

error('PsychHelperCreateRemapCLUT: Invalid/Unknown command code specified!');
end
