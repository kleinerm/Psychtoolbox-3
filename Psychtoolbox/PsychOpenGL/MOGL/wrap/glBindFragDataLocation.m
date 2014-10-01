function glBindFragDataLocation( program, color, name )

% glBindFragDataLocation  Interface to OpenGL function glBindFragDataLocation
%
% usage:  glBindFragDataLocation( program, color, name )
%
% C function:  void glBindFragDataLocation(GLuint program, GLuint color, const GLchar* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindFragDataLocation', program, color, uint8(name) );

return
