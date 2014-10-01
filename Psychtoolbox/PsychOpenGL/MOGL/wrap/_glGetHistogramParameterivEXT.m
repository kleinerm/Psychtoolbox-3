function params = glGetHistogramParameterivEXT( target, pname )

% glGetHistogramParameterivEXT  Interface to OpenGL function glGetHistogramParameterivEXT
%
% usage:  params = glGetHistogramParameterivEXT( target, pname )
%
% C function:  void glGetHistogramParameterivEXT(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetHistogramParameterivEXT', target, pname, params );

return
