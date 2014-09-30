function glEnableIndexedEXT( target, index )

% glEnableIndexedEXT  Interface to OpenGL function glEnableIndexedEXT
%
% usage:  glEnableIndexedEXT( target, index )
%
% C function:  void glEnableIndexedEXT(GLenum target, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEnableIndexedEXT', target, index );

return
