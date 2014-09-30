function glUniform2ui64NV( location, x, y )

% glUniform2ui64NV  Interface to OpenGL function glUniform2ui64NV
%
% usage:  glUniform2ui64NV( location, x, y )
%
% C function:  void glUniform2ui64NV(GLint location, GLuint64EXT x, GLuint64EXT y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2ui64NV', location, x, y );

return
