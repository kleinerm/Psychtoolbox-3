function params = glGetMapParameterivNV( target, pname )

% glGetMapParameterivNV  Interface to OpenGL function glGetMapParameterivNV
%
% usage:  params = glGetMapParameterivNV( target, pname )
%
% C function:  void glGetMapParameterivNV(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMapParameterivNV', target, pname, params );

return
