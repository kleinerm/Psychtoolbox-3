function glPixelDataRangeNV( target, length, pointer )

% glPixelDataRangeNV  Interface to OpenGL function glPixelDataRangeNV
%
% usage:  glPixelDataRangeNV( target, length, pointer )
%
% C function:  void glPixelDataRangeNV(GLenum target, GLsizei length, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPixelDataRangeNV', target, length, pointer );

return
