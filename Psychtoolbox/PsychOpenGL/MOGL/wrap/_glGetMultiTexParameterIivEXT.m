function params = glGetMultiTexParameterIivEXT( texunit, target, pname )

% glGetMultiTexParameterIivEXT  Interface to OpenGL function glGetMultiTexParameterIivEXT
%
% usage:  params = glGetMultiTexParameterIivEXT( texunit, target, pname )
%
% C function:  void glGetMultiTexParameterIivEXT(GLenum texunit, GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMultiTexParameterIivEXT', texunit, target, pname, params );

return
