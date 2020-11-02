function params = glMulticastGetQueryObjectui64vNV( gpu, id, pname )

% glMulticastGetQueryObjectui64vNV  Interface to OpenGL function glMulticastGetQueryObjectui64vNV
%
% usage:  params = glMulticastGetQueryObjectui64vNV( gpu, id, pname )
%
% C function:  void glMulticastGetQueryObjectui64vNV(GLuint gpu, GLuint id, GLenum pname, GLuint64* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glMulticastGetQueryObjectui64vNV', gpu, id, pname, params );

return
