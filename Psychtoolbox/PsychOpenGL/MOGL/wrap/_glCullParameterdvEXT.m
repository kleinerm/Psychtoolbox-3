function params = glCullParameterdvEXT( pname )

% glCullParameterdvEXT  Interface to OpenGL function glCullParameterdvEXT
%
% usage:  params = glCullParameterdvEXT( pname )
%
% C function:  void glCullParameterdvEXT(GLenum pname, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glCullParameterdvEXT', pname, params );

return
