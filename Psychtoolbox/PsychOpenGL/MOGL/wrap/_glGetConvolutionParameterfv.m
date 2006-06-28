function params = glGetConvolutionParameterfv( target, pname )

% glGetConvolutionParameterfv  Interface to OpenGL function glGetConvolutionParameterfv
%
% usage:  params = glGetConvolutionParameterfv( target, pname )
%
% C function:  void glGetConvolutionParameterfv(GLenum target, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = moglsingle(0);

moglcore( 'glGetConvolutionParameterfv', target, pname, params );

return
