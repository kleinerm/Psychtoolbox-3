function glProgramNamedParameter4fNV( id, len, name, x, y, z, w )

% glProgramNamedParameter4fNV  Interface to OpenGL function glProgramNamedParameter4fNV
%
% usage:  glProgramNamedParameter4fNV( id, len, name, x, y, z, w )
%
% C function:  void glProgramNamedParameter4fNV(GLuint id, GLsizei len, const GLubyte* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glProgramNamedParameter4fNV', id, len, uint8(name), x, y, z, w );

return
