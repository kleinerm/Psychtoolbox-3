function glUniform3ivARB( location, count, value )

% glUniform3ivARB  Interface to OpenGL function glUniform3ivARB
%
% usage:  glUniform3ivARB( location, count, value )
%
% C function:  void glUniform3ivARB(GLint location, GLsizei count, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform3ivARB', location, count, int32(value) );

return
