function glProgramLocalParameterI4iNV( target, index, x, y, z, w )

% glProgramLocalParameterI4iNV  Interface to OpenGL function glProgramLocalParameterI4iNV
%
% usage:  glProgramLocalParameterI4iNV( target, index, x, y, z, w )
%
% C function:  void glProgramLocalParameterI4iNV(GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameterI4iNV', target, index, x, y, z, w );

return
