function params = glGetMultiTexLevelParameterfvEXT( texunit, target, level, pname )

% glGetMultiTexLevelParameterfvEXT  Interface to OpenGL function glGetMultiTexLevelParameterfvEXT
%
% usage:  params = glGetMultiTexLevelParameterfvEXT( texunit, target, level, pname )
%
% C function:  void glGetMultiTexLevelParameterfvEXT(GLenum texunit, GLenum target, GLint level, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetMultiTexLevelParameterfvEXT', texunit, target, level, pname, params );

return
