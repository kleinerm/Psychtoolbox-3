function glProgramParameter4fNV( target, index, x, y, z, w )

% glProgramParameter4fNV  Interface to OpenGL function glProgramParameter4fNV
%
% usage:  glProgramParameter4fNV( target, index, x, y, z, w )
%
% C function:  void glProgramParameter4fNV(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramParameter4fNV', target, index, x, y, z, w );

return
