function params = glMulticastGetQueryObjectivNV( gpu, id, pname )

% glMulticastGetQueryObjectivNV  Interface to OpenGL function glMulticastGetQueryObjectivNV
%
% usage:  params = glMulticastGetQueryObjectivNV( gpu, id, pname )
%
% C function:  void glMulticastGetQueryObjectivNV(GLuint gpu, GLuint id, GLenum pname, GLint* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glMulticastGetQueryObjectivNV', gpu, id, pname, params );

return
