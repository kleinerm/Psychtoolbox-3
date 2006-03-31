function AssertGLSL
% AssertGLSL
%
% Break and issue an error message if the given combination of graphics
% hardware and graphics hardware driver does not support the OpenGL Shading
% Language (GLSL). This command needs to be executed after opening an
% onscreen window, because it needs a valid OpenGL context to work.
%
% HISTORY
% 3/29/06   mk     wrote it.

global GL
if isempty(GL)
    ptbmoglinit
end;

try
    extensions = glGetString(GL.EXTENSIONS);
catch
    error('AssertGLSL called before opening an Onscreen window! This will not work...');
end;

if isempty(extensions)
    error('AssertGLSL called before opening an Onscreen window! This will not work...');
end;

if isempty(strfind(extensions, 'GL_ARB_shading_language'))
    Screen('CloseAll');
    error('Sorry, this M-File can not execute on your combination of graphics hardware and driver due to lack of GLSL support.'); 
end;
