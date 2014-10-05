function glNamedProgramLocalParametersI4ivEXT( program, target, index, count, params )

% glNamedProgramLocalParametersI4ivEXT  Interface to OpenGL function glNamedProgramLocalParametersI4ivEXT
%
% usage:  glNamedProgramLocalParametersI4ivEXT( program, target, index, count, params )
%
% C function:  void glNamedProgramLocalParametersI4ivEXT(GLuint program, GLenum target, GLuint index, GLsizei count, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParametersI4ivEXT', program, target, index, count, int32(params) );

return
