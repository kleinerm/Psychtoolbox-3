function params = glGetBufferParameteri64v( target, pname )

% glGetBufferParameteri64v  Interface to OpenGL function glGetBufferParameteri64v
%
% usage:  params = glGetBufferParameteri64v( target, pname )
%
% C function:  void glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glGetBufferParameteri64v', target, pname, params );

return
