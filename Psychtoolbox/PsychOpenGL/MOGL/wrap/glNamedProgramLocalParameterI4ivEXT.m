function glNamedProgramLocalParameterI4ivEXT( program, target, index, params )

% glNamedProgramLocalParameterI4ivEXT  Interface to OpenGL function glNamedProgramLocalParameterI4ivEXT
%
% usage:  glNamedProgramLocalParameterI4ivEXT( program, target, index, params )
%
% C function:  void glNamedProgramLocalParameterI4ivEXT(GLuint program, GLenum target, GLuint index, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameterI4ivEXT', program, target, index, int32(params) );

return
