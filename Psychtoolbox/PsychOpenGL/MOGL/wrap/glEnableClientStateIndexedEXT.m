function glEnableClientStateIndexedEXT( array, index )

% glEnableClientStateIndexedEXT  Interface to OpenGL function glEnableClientStateIndexedEXT
%
% usage:  glEnableClientStateIndexedEXT( array, index )
%
% C function:  void glEnableClientStateIndexedEXT(GLenum array, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEnableClientStateIndexedEXT', array, index );

return
