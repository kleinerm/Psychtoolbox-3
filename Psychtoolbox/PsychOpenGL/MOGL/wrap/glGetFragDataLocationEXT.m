function r = glGetFragDataLocationEXT( program, name )

% glGetFragDataLocationEXT  Interface to OpenGL function glGetFragDataLocationEXT
%
% usage:  r = glGetFragDataLocationEXT( program, name )
%
% C function:  GLint glGetFragDataLocationEXT(GLuint program, const GLchar* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetFragDataLocationEXT', program, uint8(name) );

return
