function glPrimitiveRestartIndex( index )

% glPrimitiveRestartIndex  Interface to OpenGL function glPrimitiveRestartIndex
%
% usage:  glPrimitiveRestartIndex( index )
%
% C function:  void glPrimitiveRestartIndex(GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPrimitiveRestartIndex', index );

return
