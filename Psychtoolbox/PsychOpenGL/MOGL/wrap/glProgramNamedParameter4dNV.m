function glProgramNamedParameter4dNV( id, len, name, x, y, z, w )

% glProgramNamedParameter4dNV  Interface to OpenGL function glProgramNamedParameter4dNV
%
% usage:  glProgramNamedParameter4dNV( id, len, name, x, y, z, w )
%
% C function:  void glProgramNamedParameter4dNV(GLuint id, GLsizei len, const GLubyte* name, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glProgramNamedParameter4dNV', id, len, uint8(name), x, y, z, w );

return
