function r = glCreateShaderProgramv( type, count, const )

% glCreateShaderProgramv  Interface to OpenGL function glCreateShaderProgramv
%
% usage:  r = glCreateShaderProgramv( type, count, const )
%
% C function:  GLuint glCreateShaderProgramv(GLenum type, GLsizei count, const GLchar* const)

% 30-Aug-2012 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glCreateShaderProgramv', type, count, uint8(const) );

return
