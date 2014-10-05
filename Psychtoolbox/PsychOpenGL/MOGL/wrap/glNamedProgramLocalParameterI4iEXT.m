function glNamedProgramLocalParameterI4iEXT( program, target, index, x, y, z, w )

% glNamedProgramLocalParameterI4iEXT  Interface to OpenGL function glNamedProgramLocalParameterI4iEXT
%
% usage:  glNamedProgramLocalParameterI4iEXT( program, target, index, x, y, z, w )
%
% C function:  void glNamedProgramLocalParameterI4iEXT(GLuint program, GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameterI4iEXT', program, target, index, x, y, z, w );

return
