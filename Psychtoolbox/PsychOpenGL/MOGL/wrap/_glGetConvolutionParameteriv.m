function params = glGetConvolutionParameteriv( target, pname )

% glGetConvolutionParameteriv  Interface to OpenGL function glGetConvolutionParameteriv
%
% usage:  params = glGetConvolutionParameteriv( target, pname )
%
% C function:  void glGetConvolutionParameteriv(GLenum target, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetConvolutionParameteriv', target, pname, params );

return
