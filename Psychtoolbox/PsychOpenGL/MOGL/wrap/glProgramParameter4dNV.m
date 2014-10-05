function glProgramParameter4dNV( target, index, x, y, z, w )

% glProgramParameter4dNV  Interface to OpenGL function glProgramParameter4dNV
%
% usage:  glProgramParameter4dNV( target, index, x, y, z, w )
%
% C function:  void glProgramParameter4dNV(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramParameter4dNV', target, index, x, y, z, w );

return
