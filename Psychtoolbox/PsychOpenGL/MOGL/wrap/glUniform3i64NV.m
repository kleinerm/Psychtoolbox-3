function glUniform3i64NV( location, x, y, z )

% glUniform3i64NV  Interface to OpenGL function glUniform3i64NV
%
% usage:  glUniform3i64NV( location, x, y, z )
%
% C function:  void glUniform3i64NV(GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniform3i64NV', location, x, y, z );

return
