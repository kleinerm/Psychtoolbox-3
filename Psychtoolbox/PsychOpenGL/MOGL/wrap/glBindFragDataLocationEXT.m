function glBindFragDataLocationEXT( program, color, name )

% glBindFragDataLocationEXT  Interface to OpenGL function glBindFragDataLocationEXT
%
% usage:  glBindFragDataLocationEXT( program, color, name )
%
% C function:  void glBindFragDataLocationEXT(GLuint program, GLuint color, const GLchar* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindFragDataLocationEXT', program, color, uint8(name) );

return
