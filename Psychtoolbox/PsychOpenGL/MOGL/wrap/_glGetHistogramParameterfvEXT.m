function params = glGetHistogramParameterfvEXT( target, pname )

% glGetHistogramParameterfvEXT  Interface to OpenGL function glGetHistogramParameterfvEXT
%
% usage:  params = glGetHistogramParameterfvEXT( target, pname )
%
% C function:  void glGetHistogramParameterfvEXT(GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetHistogramParameterfvEXT', target, pname, params );

return
