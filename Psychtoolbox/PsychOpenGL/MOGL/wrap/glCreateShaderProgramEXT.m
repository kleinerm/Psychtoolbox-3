function r = glCreateShaderProgramEXT( type, string )

% glCreateShaderProgramEXT  Interface to OpenGL function glCreateShaderProgramEXT
%
% usage:  r = glCreateShaderProgramEXT( type, string )
%
% C function:  GLuint glCreateShaderProgramEXT(GLenum type, const GLchar* string)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glCreateShaderProgramEXT', type, uint8(string) );

return
