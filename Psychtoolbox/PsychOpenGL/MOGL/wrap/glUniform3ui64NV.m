function glUniform3ui64NV( location, x, y, z )

% glUniform3ui64NV  Interface to OpenGL function glUniform3ui64NV
%
% usage:  glUniform3ui64NV( location, x, y, z )
%
% C function:  void glUniform3ui64NV(GLint location, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniform3ui64NV', location, x, y, z );

return
