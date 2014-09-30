function params = glGetQueryObjectivARB( id, pname )

% glGetQueryObjectivARB  Interface to OpenGL function glGetQueryObjectivARB
%
% usage:  params = glGetQueryObjectivARB( id, pname )
%
% C function:  void glGetQueryObjectivARB(GLuint id, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetQueryObjectivARB', id, pname, params );

return
