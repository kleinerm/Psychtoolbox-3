function data = glGetDoubleIndexedvEXT( target, index )

% glGetDoubleIndexedvEXT  Interface to OpenGL function glGetDoubleIndexedvEXT
%
% usage:  data = glGetDoubleIndexedvEXT( target, index )
%
% C function:  void glGetDoubleIndexedvEXT(GLenum target, GLuint index, GLdouble* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = double(0);

moglcore( 'glGetDoubleIndexedvEXT', target, index, data );

return
