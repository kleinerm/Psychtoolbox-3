function params = glMulticastGetQueryObjectuivNV( gpu, id, pname )

% glMulticastGetQueryObjectuivNV  Interface to OpenGL function glMulticastGetQueryObjectuivNV
%
% usage:  params = glMulticastGetQueryObjectuivNV( gpu, id, pname )
%
% C function:  void glMulticastGetQueryObjectuivNV(GLuint gpu, GLuint id, GLenum pname, GLuint* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glMulticastGetQueryObjectuivNV', gpu, id, pname, params );

return
