function data = glGetFloatIndexedvEXT( target, index )

% glGetFloatIndexedvEXT  Interface to OpenGL function glGetFloatIndexedvEXT
%
% usage:  data = glGetFloatIndexedvEXT( target, index )
%
% C function:  void glGetFloatIndexedvEXT(GLenum target, GLuint index, GLfloat* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = single(0);

moglcore( 'glGetFloatIndexedvEXT', target, index, data );

return
