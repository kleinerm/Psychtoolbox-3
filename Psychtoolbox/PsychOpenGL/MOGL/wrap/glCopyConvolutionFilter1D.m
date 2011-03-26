function glCopyConvolutionFilter1D( target, internalformat, x, y, width )

% glCopyConvolutionFilter1D  Interface to OpenGL function glCopyConvolutionFilter1D
%
% usage:  glCopyConvolutionFilter1D( target, internalformat, x, y, width )
%
% C function:  void glCopyConvolutionFilter1D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glCopyConvolutionFilter1D', target, internalformat, x, y, width );

return
