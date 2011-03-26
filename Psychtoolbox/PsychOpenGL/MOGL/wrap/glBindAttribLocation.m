function glBindAttribLocation( program, index, name )

% glBindAttribLocation  Interface to OpenGL function glBindAttribLocation
%
% usage:  glBindAttribLocation( program, index, name )
%
% C function:  void glBindAttribLocation(GLuint program, GLuint index, const GLchar* name)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindAttribLocation', program, index, uint8(name) );

return
