function glProgramEnvParameterI4iNV( target, index, x, y, z, w )

% glProgramEnvParameterI4iNV  Interface to OpenGL function glProgramEnvParameterI4iNV
%
% usage:  glProgramEnvParameterI4iNV( target, index, x, y, z, w )
%
% C function:  void glProgramEnvParameterI4iNV(GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameterI4iNV', target, index, x, y, z, w );

return
