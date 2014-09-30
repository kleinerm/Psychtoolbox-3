function params = glGetQueryObjectuivARB( id, pname )

% glGetQueryObjectuivARB  Interface to OpenGL function glGetQueryObjectuivARB
%
% usage:  params = glGetQueryObjectuivARB( id, pname )
%
% C function:  void glGetQueryObjectuivARB(GLuint id, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetQueryObjectuivARB', id, pname, params );

return
