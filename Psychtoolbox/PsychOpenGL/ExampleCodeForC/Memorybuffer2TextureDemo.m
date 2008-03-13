function Memorybuffer2TextureDemo
% Memorybuffer2TextureDemo
%
% You have to build the required MEX or OCT file
% Memorybuffer2Texture.c or Memorybuffer2Texture.cc
% from source code.
%
% Have a look at its C or C++ code and do it. If you don't
% know how to do that, this is not for you!
%
% This demo demonstrates how to create a image in a
% memory buffer inside a C Mex-file or C++ Oct-file and
% pass the image data as a memory pointer to Psychtoolbox
% to update or build a Psychtoolbox OpenGL texture from it.
%
% --> Injection of images into PTB.
%
% The demo will display a simple colorful thingy,
% created in C code and injected as PTB texture.
%
% Press any key to exit.

% History:
% 23.6.2006 Written (MK).

% We skip all sync tests and calibrations: If you use this in
% real experiment code, i'll feel very sorry for you...
Screen('Preference', 'SkipSyncTests', 2);

InitializeMatlabOpenGL([], 0, 1);

% Open window on screen, clear it to background color black:
screenid = max(Screen('Screens'));
win = Screen('OpenWindow', screenid, 0);

% Call our simple mex-file. It will create a 640 by 480
% 3 bytes per pixel color image and return a specially
% encoded memory pointer to its in-core representation.
% This file is so simple, it does not take any parameters.
imgptr = Memorybuffer2Texture;

% Inject the 3 channel image into Psychtoolbox, create a new texture and return a handle to it.
% Change the 0-flag 'upsidedown' to 1, if your images appear upside down.
texid = Screen('SetOpenGLTextureFromMemPointer', win, [], imgptr, 640, 480, 4, 0, [], GL.RGBA, GL.RGB, GL.UNSIGNED_BYTE);

% Draw texture centered in window:
Screen('DrawTexture', win, texid);

% Show it:
Screen('Flip', win);

% Wait for keypress:
KbWait;

% That's it. Close windows and exit.
Screen('CloseAll');
return;
