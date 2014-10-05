function params = glGetOcclusionQueryuivNV( id, pname )

% glGetOcclusionQueryuivNV  Interface to OpenGL function glGetOcclusionQueryuivNV
%
% usage:  params = glGetOcclusionQueryuivNV( id, pname )
%
% C function:  void glGetOcclusionQueryuivNV(GLuint id, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetOcclusionQueryuivNV', id, pname, params );

return
