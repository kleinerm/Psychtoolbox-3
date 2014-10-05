function glColorMaskIndexedEXT( index, r, g, b, a )

% glColorMaskIndexedEXT  Interface to OpenGL function glColorMaskIndexedEXT
%
% usage:  glColorMaskIndexedEXT( index, r, g, b, a )
%
% C function:  void glColorMaskIndexedEXT(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glColorMaskIndexedEXT', index, r, g, b, a );

return
