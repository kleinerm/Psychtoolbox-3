function param = glGetVertexArrayIntegeri_vEXT( vaobj, index, pname )

% glGetVertexArrayIntegeri_vEXT  Interface to OpenGL function glGetVertexArrayIntegeri_vEXT
%
% usage:  param = glGetVertexArrayIntegeri_vEXT( vaobj, index, pname )
%
% C function:  void glGetVertexArrayIntegeri_vEXT(GLuint vaobj, GLuint index, GLenum pname, GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

param = int32(0);

moglcore( 'glGetVertexArrayIntegeri_vEXT', vaobj, index, pname, param );

return
