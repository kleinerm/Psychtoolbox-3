function params = glMulticastGetQueryObjecti64vNV( gpu, id, pname )

% glMulticastGetQueryObjecti64vNV  Interface to OpenGL function glMulticastGetQueryObjecti64vNV
%
% usage:  params = glMulticastGetQueryObjecti64vNV( gpu, id, pname )
%
% C function:  void glMulticastGetQueryObjecti64vNV(GLuint gpu, GLuint id, GLenum pname, GLint64* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glMulticastGetQueryObjecti64vNV', gpu, id, pname, params );

return
