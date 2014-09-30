function glFlushPixelDataRangeNV( target )

% glFlushPixelDataRangeNV  Interface to OpenGL function glFlushPixelDataRangeNV
%
% usage:  glFlushPixelDataRangeNV( target )
%
% C function:  void glFlushPixelDataRangeNV(GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFlushPixelDataRangeNV', target );

return
