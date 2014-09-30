function r = glGetFragDataLocation( program, name )

% glGetFragDataLocation  Interface to OpenGL function glGetFragDataLocation
%
% usage:  r = glGetFragDataLocation( program, name )
%
% C function:  GLint glGetFragDataLocation(GLuint program, const GLchar* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetFragDataLocation', program, uint8(name) );

return
