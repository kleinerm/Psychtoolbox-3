function params = glGetConvolutionParameterfv( target, pname )

% glGetConvolutionParameterfv  Interface to OpenGL function glGetConvolutionParameterfv
%
% usage:  params = glGetConvolutionParameterfv( target, pname )
%
% C function:  void glGetConvolutionParameterfv(GLenum target, GLenum pname, GLfloat* params)

% 25-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetConvolutionParameterfv', target, pname, params );

return
