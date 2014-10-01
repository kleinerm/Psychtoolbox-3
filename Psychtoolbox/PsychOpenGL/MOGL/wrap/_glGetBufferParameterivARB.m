function params = glGetBufferParameterivARB( target, pname )

% glGetBufferParameterivARB  Interface to OpenGL function glGetBufferParameterivARB
%
% usage:  params = glGetBufferParameterivARB( target, pname )
%
% C function:  void glGetBufferParameterivARB(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetBufferParameterivARB', target, pname, params );

return
