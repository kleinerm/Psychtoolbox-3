function glNamedProgramLocalParameterI4uiEXT( program, target, index, x, y, z, w )

% glNamedProgramLocalParameterI4uiEXT  Interface to OpenGL function glNamedProgramLocalParameterI4uiEXT
%
% usage:  glNamedProgramLocalParameterI4uiEXT( program, target, index, x, y, z, w )
%
% C function:  void glNamedProgramLocalParameterI4uiEXT(GLuint program, GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameterI4uiEXT', program, target, index, x, y, z, w );

return
