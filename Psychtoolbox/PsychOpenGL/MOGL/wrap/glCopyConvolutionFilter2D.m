function glCopyConvolutionFilter2D( target, internalformat, x, y, width, height )

% glCopyConvolutionFilter2D  Interface to OpenGL function glCopyConvolutionFilter2D
%
% usage:  glCopyConvolutionFilter2D( target, internalformat, x, y, width, height )
%
% C function:  void glCopyConvolutionFilter2D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glCopyConvolutionFilter2D', target, internalformat, x, y, width, height );

return
