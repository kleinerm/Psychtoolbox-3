function glDisableIndexedEXT( target, index )

% glDisableIndexedEXT  Interface to OpenGL function glDisableIndexedEXT
%
% usage:  glDisableIndexedEXT( target, index )
%
% C function:  void glDisableIndexedEXT(GLenum target, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDisableIndexedEXT', target, index );

return
