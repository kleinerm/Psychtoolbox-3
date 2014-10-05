function glDisableClientStateIndexedEXT( array, index )

% glDisableClientStateIndexedEXT  Interface to OpenGL function glDisableClientStateIndexedEXT
%
% usage:  glDisableClientStateIndexedEXT( array, index )
%
% C function:  void glDisableClientStateIndexedEXT(GLenum array, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDisableClientStateIndexedEXT', array, index );

return
