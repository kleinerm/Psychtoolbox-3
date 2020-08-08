function params = glGetMemoryObjectParameterivEXT( memoryObject, pname )

% glGetMemoryObjectParameterivEXT  Interface to OpenGL function glGetMemoryObjectParameterivEXT
%
% usage:  params = glGetMemoryObjectParameterivEXT( memoryObject, pname )
%
% C function:  void glGetMemoryObjectParameterivEXT(GLuint memoryObject, GLenum pname, GLint* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMemoryObjectParameterivEXT', memoryObject, pname, params );

return
