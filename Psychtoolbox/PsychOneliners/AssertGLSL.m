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

if isempty(findstr(extensions, 'GL_ARB_shading_language')) | isempty(findstr(extensions, 'GL_ARB_shader_objects'))
    Screen('CloseAll');
    error('Sorry, this M-File can not execute on your combination of graphics hardware and driver due to lack of GLSL support.'); 
end;

if isempty(findstr(extensions, 'GL_ARB_fragment_shader'))
   fprintf('AssertGLSL: Warning. Your graphics hardware does not support fragment shaders. This will severely limit the use of GLSL.\n');
   fprintf('AssertGLSL: Many image processing functions will fail.\n');
end;
