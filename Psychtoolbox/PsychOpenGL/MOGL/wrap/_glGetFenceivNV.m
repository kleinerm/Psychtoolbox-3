function params = glGetFenceivNV( fence, pname )

% glGetFenceivNV  Interface to OpenGL function glGetFenceivNV
%
% usage:  params = glGetFenceivNV( fence, pname )
%
% C function:  void glGetFenceivNV(GLuint fence, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetFenceivNV', fence, pname, params );

return
