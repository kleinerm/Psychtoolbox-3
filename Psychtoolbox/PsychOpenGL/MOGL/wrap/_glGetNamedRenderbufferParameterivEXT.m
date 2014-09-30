function params = glGetNamedRenderbufferParameterivEXT( renderbuffer, pname )

% glGetNamedRenderbufferParameterivEXT  Interface to OpenGL function glGetNamedRenderbufferParameterivEXT
%
% usage:  params = glGetNamedRenderbufferParameterivEXT( renderbuffer, pname )
%
% C function:  void glGetNamedRenderbufferParameterivEXT(GLuint renderbuffer, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetNamedRenderbufferParameterivEXT', renderbuffer, pname, params );

return
