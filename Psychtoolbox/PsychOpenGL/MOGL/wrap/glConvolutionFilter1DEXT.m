function glConvolutionFilter1DEXT( target, internalformat, width, format, type, image )

% glConvolutionFilter1DEXT  Interface to OpenGL function glConvolutionFilter1DEXT
%
% usage:  glConvolutionFilter1DEXT( target, internalformat, width, format, type, image )
%
% C function:  void glConvolutionFilter1DEXT(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const void* image)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionFilter1DEXT', target, internalformat, width, format, type, image );

return
