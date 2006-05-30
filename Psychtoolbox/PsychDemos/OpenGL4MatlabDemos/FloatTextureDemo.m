function FloatTextureDemo

% Is the script running in OpenGL Psychtoolbox?
AssertOpenGL;

% Find the screen to use for display:
screenid=max(Screen('Screens'));

% Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
% mogl OpenGL for Matlab wrapper:
InitializeMatlabOpenGL(1);

% Open a double-buffered full-screen window on the main displays screen.
[win , winRect] = Screen('OpenWindow', screenid);

% Setup the OpenGL rendering context of the onscreen window for use by
% OpenGL wrapper. After this command, all following OpenGL commands will
% draw into the onscreen window 'win':
Screen('BeginOpenGL', win);

% Enable 2D texture mapping, so the faces of the cube will show some nice
% images:
glEnable(GL_TEXTURE_RECTANGLE_EXT);

% Generate 2 textures and store their handles in vector 'texname'
texname=glGenTextures(2);

% Create two framebuffer objects:
fbos = glGenFramebuffersEXT(2);

% Setup textures for all six sides of cube:
for i=1:2,
    % Enable i'th texture by binding it:
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT,texname(i));
    
    % Create i'th textures representation:
    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA_FLOAT32_APPLE, 256, 256, 0, GL_RGBA, GL_FLOAT, 0);

    % Setup texture wrapping behaviour:
    glTexParameterfv(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameterfv(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_WRAP_T,GL_CLAMP);

    % Setup filtering for the textures:
    glTexParameterfv(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameterfv(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    % Choose texture application function:
    glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

    % Unbind it after setup:
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
    
    % Bind i'th fbo:
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbos(i));

    % Attch i'th texture as color buffer to i'th framebuffer object:
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT , GL_TEXTURE_RECTANGLE_EXT, texname(i), 0);
    
    % Check if ok:
    fbostatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    
    if not(fbostatus == GL_FRAMEBUFFER_COMPLETE_EXT)
        Screen('CloseAll');
        disp(fbostatus);
        error('Failed to setup framebuffer object!');
    end;
    
    % Unbind it:
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
end

% Ok, at this point we should have 2 FBO's with floating point textures
% attached as color buffers - ready for render to floatinpoint target:
    
% Finish OpenGL rendering into PTB window and check for OpenGL errors.
Screen('EndOpenGL', win);

Screen('FillRect', win, [255 255 0]);
% Show rendered image at next vertical retrace:
Screen('Flip', win);

KbWait;

img = glReadPixels(0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE);


% Switch to standard framebuffer:
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

% Delete all allocated OpenGL textures:
glDeleteTextures(length(texname),texname);

% Delete all FBO's:
glDeleteFramebuffersEXT(length(fbos), fbos);

% Shut down OpenGL rendering:
Screen('EndOpenGL', win);

% Close onscreen window and release all other ressources:
Screen('CloseAll');

imshow(img);
% Well done!
return
