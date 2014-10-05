function glUniform1ivARB( location, count, value )

% glUniform1ivARB  Interface to OpenGL function glUniform1ivARB
%
% usage:  glUniform1ivARB( location, count, value )
%
% C function:  void glUniform1ivARB(GLint location, GLsizei count, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform1ivARB', location, count, int32(value) );

return
