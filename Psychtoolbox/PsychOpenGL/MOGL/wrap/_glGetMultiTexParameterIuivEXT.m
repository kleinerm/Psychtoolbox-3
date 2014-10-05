function params = glGetMultiTexParameterIuivEXT( texunit, target, pname )

% glGetMultiTexParameterIuivEXT  Interface to OpenGL function glGetMultiTexParameterIuivEXT
%
% usage:  params = glGetMultiTexParameterIuivEXT( texunit, target, pname )
%
% C function:  void glGetMultiTexParameterIuivEXT(GLenum texunit, GLenum target, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetMultiTexParameterIuivEXT', texunit, target, pname, params );

return
