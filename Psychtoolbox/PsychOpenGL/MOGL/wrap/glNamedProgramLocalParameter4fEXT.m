function glNamedProgramLocalParameter4fEXT( program, target, index, x, y, z, w )

% glNamedProgramLocalParameter4fEXT  Interface to OpenGL function glNamedProgramLocalParameter4fEXT
%
% usage:  glNamedProgramLocalParameter4fEXT( program, target, index, x, y, z, w )
%
% C function:  void glNamedProgramLocalParameter4fEXT(GLuint program, GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameter4fEXT', program, target, index, x, y, z, w );

return
