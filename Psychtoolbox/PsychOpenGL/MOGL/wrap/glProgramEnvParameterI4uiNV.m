function glProgramEnvParameterI4uiNV( target, index, x, y, z, w )

% glProgramEnvParameterI4uiNV  Interface to OpenGL function glProgramEnvParameterI4uiNV
%
% usage:  glProgramEnvParameterI4uiNV( target, index, x, y, z, w )
%
% C function:  void glProgramEnvParameterI4uiNV(GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameterI4uiNV', target, index, x, y, z, w );

return
