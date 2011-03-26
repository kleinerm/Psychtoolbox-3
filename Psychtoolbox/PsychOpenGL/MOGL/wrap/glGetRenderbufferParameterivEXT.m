function params = glGetRenderbufferParameterivEXT( target, pname )

% glGetRenderbufferParameterivEXT  Interface to OpenGL function glGetRenderbufferParameterivEXT
%
% usage:  params = glGetRenderbufferParameterivEXT( target, pname )
%
% C function:  void glGetRenderbufferParameterivEXT(GLenum target, GLenum pname, GLint* params)

% 30-May-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(1);

moglcore( 'glGetRenderbufferParameterivEXT', target, pname, params );

return
% ---skip---
