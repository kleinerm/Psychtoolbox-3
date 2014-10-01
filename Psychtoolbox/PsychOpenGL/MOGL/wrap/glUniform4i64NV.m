function glUniform4i64NV( location, x, y, z, w )

% glUniform4i64NV  Interface to OpenGL function glUniform4i64NV
%
% usage:  glUniform4i64NV( location, x, y, z, w )
%
% C function:  void glUniform4i64NV(GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z, GLint64EXT w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4i64NV', location, x, y, z, w );

return
