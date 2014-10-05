function glCopyConvolutionFilter1DEXT( target, internalformat, x, y, width )

% glCopyConvolutionFilter1DEXT  Interface to OpenGL function glCopyConvolutionFilter1DEXT
%
% usage:  glCopyConvolutionFilter1DEXT( target, internalformat, x, y, width )
%
% C function:  void glCopyConvolutionFilter1DEXT(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glCopyConvolutionFilter1DEXT', target, internalformat, x, y, width );

return
