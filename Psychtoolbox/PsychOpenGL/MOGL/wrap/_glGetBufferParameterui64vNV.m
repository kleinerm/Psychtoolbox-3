function params = glGetBufferParameterui64vNV( target, pname )

% glGetBufferParameterui64vNV  Interface to OpenGL function glGetBufferParameterui64vNV
%
% usage:  params = glGetBufferParameterui64vNV( target, pname )
%
% C function:  void glGetBufferParameterui64vNV(GLenum target, GLenum pname, GLuint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetBufferParameterui64vNV', target, pname, params );

return
