function params = glGetMapAttribParameterivNV( target, index, pname )

% glGetMapAttribParameterivNV  Interface to OpenGL function glGetMapAttribParameterivNV
%
% usage:  params = glGetMapAttribParameterivNV( target, index, pname )
%
% C function:  void glGetMapAttribParameterivNV(GLenum target, GLuint index, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMapAttribParameterivNV', target, index, pname, params );

return
