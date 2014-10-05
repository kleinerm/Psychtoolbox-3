function glCopyConvolutionFilter2DEXT( target, internalformat, x, y, width, height )

% glCopyConvolutionFilter2DEXT  Interface to OpenGL function glCopyConvolutionFilter2DEXT
%
% usage:  glCopyConvolutionFilter2DEXT( target, internalformat, x, y, width, height )
%
% C function:  void glCopyConvolutionFilter2DEXT(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glCopyConvolutionFilter2DEXT', target, internalformat, x, y, width, height );

return
