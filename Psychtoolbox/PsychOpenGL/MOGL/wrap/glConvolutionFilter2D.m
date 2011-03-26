function glConvolutionFilter2D( target, internalformat, width, height, format, type, image )

% glConvolutionFilter2D  Interface to OpenGL function glConvolutionFilter2D
%
% usage:  glConvolutionFilter2D( target, internalformat, width, height, format, type, image )
%
% C function:  void glConvolutionFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* image)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionFilter2D', target, internalformat, width, height, format, type, image );

return
