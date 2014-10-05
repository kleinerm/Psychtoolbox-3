function params = glGetMultiTexEnvfvEXT( texunit, target, pname )

% glGetMultiTexEnvfvEXT  Interface to OpenGL function glGetMultiTexEnvfvEXT
%
% usage:  params = glGetMultiTexEnvfvEXT( texunit, target, pname )
%
% C function:  void glGetMultiTexEnvfvEXT(GLenum texunit, GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetMultiTexEnvfvEXT', texunit, target, pname, params );

return
