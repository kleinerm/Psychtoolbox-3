function glConvolutionFilter2DEXT( target, internalformat, width, height, format, type, image )

% glConvolutionFilter2DEXT  Interface to OpenGL function glConvolutionFilter2DEXT
%
% usage:  glConvolutionFilter2DEXT( target, internalformat, width, height, format, type, image )
%
% C function:  void glConvolutionFilter2DEXT(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* image)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionFilter2DEXT', target, internalformat, width, height, format, type, image );

return
