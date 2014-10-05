function glCurrentPaletteMatrixARB( index )

% glCurrentPaletteMatrixARB  Interface to OpenGL function glCurrentPaletteMatrixARB
%
% usage:  glCurrentPaletteMatrixARB( index )
%
% C function:  void glCurrentPaletteMatrixARB(GLint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glCurrentPaletteMatrixARB', index );

return
