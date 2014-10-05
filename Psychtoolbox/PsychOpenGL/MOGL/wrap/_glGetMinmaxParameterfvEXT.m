function params = glGetMinmaxParameterfvEXT( target, pname )

% glGetMinmaxParameterfvEXT  Interface to OpenGL function glGetMinmaxParameterfvEXT
%
% usage:  params = glGetMinmaxParameterfvEXT( target, pname )
%
% C function:  void glGetMinmaxParameterfvEXT(GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetMinmaxParameterfvEXT', target, pname, params );

return
