function params = glGetHistogramParameterfv( target, pname )

% glGetHistogramParameterfv  Interface to OpenGL function glGetHistogramParameterfv
%
% usage:  params = glGetHistogramParameterfv( target, pname )
%
% C function:  void glGetHistogramParameterfv(GLenum target, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = moglsingle(0);

moglcore( 'glGetHistogramParameterfv', target, pname, params );

return
