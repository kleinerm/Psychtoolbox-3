function params = glGetQueryObjecti64vEXT( id, pname )

% glGetQueryObjecti64vEXT  Interface to OpenGL function glGetQueryObjecti64vEXT
%
% usage:  params = glGetQueryObjecti64vEXT( id, pname )
%
% C function:  void glGetQueryObjecti64vEXT(GLuint id, GLenum pname, GLint64* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glGetQueryObjecti64vEXT', id, pname, params );

return
