function glMemoryObjectParameterivEXT( memoryObject, pname, params )

% glMemoryObjectParameterivEXT  Interface to OpenGL function glMemoryObjectParameterivEXT
%
% usage:  glMemoryObjectParameterivEXT( memoryObject, pname, params )
%
% C function:  void glMemoryObjectParameterivEXT(GLuint memoryObject, GLenum pname, const GLint* params)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMemoryObjectParameterivEXT', memoryObject, pname, int32(params) );

return
