function glConvolutionFilter1D( target, internalformat, width, format, type, image )

% glConvolutionFilter1D  Interface to OpenGL function glConvolutionFilter1D
%
% usage:  glConvolutionFilter1D( target, internalformat, width, format, type, image )
%
% C function:  void glConvolutionFilter1D(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid* image)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionFilter1D', target, internalformat, width, format, type, image );

return
