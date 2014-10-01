function glUniform2i64NV( location, x, y )

% glUniform2i64NV  Interface to OpenGL function glUniform2i64NV
%
% usage:  glUniform2i64NV( location, x, y )
%
% C function:  void glUniform2i64NV(GLint location, GLint64EXT x, GLint64EXT y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2i64NV', location, x, y );

return
