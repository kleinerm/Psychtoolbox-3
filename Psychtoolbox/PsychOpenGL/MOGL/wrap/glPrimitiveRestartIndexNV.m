function glPrimitiveRestartIndexNV( index )

% glPrimitiveRestartIndexNV  Interface to OpenGL function glPrimitiveRestartIndexNV
%
% usage:  glPrimitiveRestartIndexNV( index )
%
% C function:  void glPrimitiveRestartIndexNV(GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPrimitiveRestartIndexNV', index );

return
