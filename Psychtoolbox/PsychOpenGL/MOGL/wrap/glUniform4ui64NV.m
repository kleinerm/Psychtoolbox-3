function glUniform4ui64NV( location, x, y, z, w )

% glUniform4ui64NV  Interface to OpenGL function glUniform4ui64NV
%
% usage:  glUniform4ui64NV( location, x, y, z, w )
%
% C function:  void glUniform4ui64NV(GLint location, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z, GLuint64EXT w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4ui64NV', location, x, y, z, w );

return
