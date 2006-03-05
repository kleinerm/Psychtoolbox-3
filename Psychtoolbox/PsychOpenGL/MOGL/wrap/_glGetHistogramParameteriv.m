function params = glGetHistogramParameteriv( target, pname )

% glGetHistogramParameteriv  Interface to OpenGL function glGetHistogramParameteriv
%
% usage:  params = glGetHistogramParameteriv( target, pname )
%
% C function:  void glGetHistogramParameteriv(GLenum target, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetHistogramParameteriv', target, pname, params );

return
