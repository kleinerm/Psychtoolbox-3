function params = glGetNamedRenderbufferParameteriv( renderbuffer, pname )

% glGetNamedRenderbufferParameteriv  Interface to OpenGL function glGetNamedRenderbufferParameteriv
%
% usage:  params = glGetNamedRenderbufferParameteriv( renderbuffer, pname )
%
% C function:  void glGetNamedRenderbufferParameteriv(GLuint renderbuffer, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetNamedRenderbufferParameteriv', renderbuffer, pname, params );

return
