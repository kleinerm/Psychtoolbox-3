function data = glGetBooleanIndexedvEXT( target, index )

% glGetBooleanIndexedvEXT  Interface to OpenGL function glGetBooleanIndexedvEXT
%
% usage:  data = glGetBooleanIndexedvEXT( target, index )
%
% C function:  void glGetBooleanIndexedvEXT(GLenum target, GLuint index, GLboolean* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = uint8(0);

moglcore( 'glGetBooleanIndexedvEXT', target, index, data );

return
