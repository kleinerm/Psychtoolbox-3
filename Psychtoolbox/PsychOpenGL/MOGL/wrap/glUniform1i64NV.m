function glUniform1i64NV( location, x )

% glUniform1i64NV  Interface to OpenGL function glUniform1i64NV
%
% usage:  glUniform1i64NV( location, x )
%
% C function:  void glUniform1i64NV(GLint location, GLint64EXT x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1i64NV', location, x );

return
