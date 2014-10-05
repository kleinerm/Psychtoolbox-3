function params = glGetQueryObjectui64vEXT( id, pname )

% glGetQueryObjectui64vEXT  Interface to OpenGL function glGetQueryObjectui64vEXT
%
% usage:  params = glGetQueryObjectui64vEXT( id, pname )
%
% C function:  void glGetQueryObjectui64vEXT(GLuint id, GLenum pname, GLuint64* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetQueryObjectui64vEXT', id, pname, params );

return
