function params = glCullParameterfvEXT( pname )

% glCullParameterfvEXT  Interface to OpenGL function glCullParameterfvEXT
%
% usage:  params = glCullParameterfvEXT( pname )
%
% C function:  void glCullParameterfvEXT(GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glCullParameterfvEXT', pname, params );

return
