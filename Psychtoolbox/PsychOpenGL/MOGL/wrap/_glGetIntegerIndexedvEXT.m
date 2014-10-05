function data = glGetIntegerIndexedvEXT( target, index )

% glGetIntegerIndexedvEXT  Interface to OpenGL function glGetIntegerIndexedvEXT
%
% usage:  data = glGetIntegerIndexedvEXT( target, index )
%
% C function:  void glGetIntegerIndexedvEXT(GLenum target, GLuint index, GLint* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = int32(0);

moglcore( 'glGetIntegerIndexedvEXT', target, index, data );

return
