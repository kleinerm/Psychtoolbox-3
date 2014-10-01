function glNamedProgramLocalParameter4dEXT( program, target, index, x, y, z, w )

% glNamedProgramLocalParameter4dEXT  Interface to OpenGL function glNamedProgramLocalParameter4dEXT
%
% usage:  glNamedProgramLocalParameter4dEXT( program, target, index, x, y, z, w )
%
% C function:  void glNamedProgramLocalParameter4dEXT(GLuint program, GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameter4dEXT', program, target, index, x, y, z, w );

return
