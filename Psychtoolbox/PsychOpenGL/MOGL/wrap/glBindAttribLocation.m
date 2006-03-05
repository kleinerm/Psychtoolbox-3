function glBindAttribLocation( program, index, name )

% glBindAttribLocation  Interface to OpenGL function glBindAttribLocation
%
% usage:  glBindAttribLocation( program, index, name )
%
% C function:  void glBindAttribLocation(GLuint program, GLuint index, const GLchar* name)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindAttribLocation', program, index, uint8(name) );

return
