function params = glGetMultiTexParameterfvEXT( texunit, target, pname )

% glGetMultiTexParameterfvEXT  Interface to OpenGL function glGetMultiTexParameterfvEXT
%
% usage:  params = glGetMultiTexParameterfvEXT( texunit, target, pname )
%
% C function:  void glGetMultiTexParameterfvEXT(GLenum texunit, GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetMultiTexParameterfvEXT', texunit, target, pname, params );

return
