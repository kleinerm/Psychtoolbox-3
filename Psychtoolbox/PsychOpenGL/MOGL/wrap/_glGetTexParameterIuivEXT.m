function params = glGetTexParameterIuivEXT( target, pname )

% glGetTexParameterIuivEXT  Interface to OpenGL function glGetTexParameterIuivEXT
%
% usage:  params = glGetTexParameterIuivEXT( target, pname )
%
% C function:  void glGetTexParameterIuivEXT(GLenum target, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetTexParameterIuivEXT', target, pname, params );

return
