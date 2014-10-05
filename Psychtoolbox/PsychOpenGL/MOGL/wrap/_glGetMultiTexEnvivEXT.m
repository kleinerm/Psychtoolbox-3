function params = glGetMultiTexEnvivEXT( texunit, target, pname )

% glGetMultiTexEnvivEXT  Interface to OpenGL function glGetMultiTexEnvivEXT
%
% usage:  params = glGetMultiTexEnvivEXT( texunit, target, pname )
%
% C function:  void glGetMultiTexEnvivEXT(GLenum texunit, GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMultiTexEnvivEXT', texunit, target, pname, params );

return
