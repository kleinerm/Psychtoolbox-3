function glConvolutionParameterfv( target, pname, params )

% glConvolutionParameterfv  Interface to OpenGL function glConvolutionParameterfv
%
% usage:  glConvolutionParameterfv( target, pname, params )
%
% C function:  void glConvolutionParameterfv(GLenum target, GLenum pname, const GLfloat* params)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionParameterfv', target, pname, single(params) );

return
