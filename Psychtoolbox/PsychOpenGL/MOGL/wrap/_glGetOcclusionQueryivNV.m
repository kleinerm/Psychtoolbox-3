function params = glGetOcclusionQueryivNV( id, pname )

% glGetOcclusionQueryivNV  Interface to OpenGL function glGetOcclusionQueryivNV
%
% usage:  params = glGetOcclusionQueryivNV( id, pname )
%
% C function:  void glGetOcclusionQueryivNV(GLuint id, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetOcclusionQueryivNV', id, pname, params );

return
