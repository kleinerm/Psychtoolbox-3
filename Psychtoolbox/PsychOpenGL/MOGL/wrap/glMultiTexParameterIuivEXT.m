function glMultiTexParameterIuivEXT( texunit, target, pname, params )

% glMultiTexParameterIuivEXT  Interface to OpenGL function glMultiTexParameterIuivEXT
%
% usage:  glMultiTexParameterIuivEXT( texunit, target, pname, params )
%
% C function:  void glMultiTexParameterIuivEXT(GLenum texunit, GLenum target, GLenum pname, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexParameterIuivEXT', texunit, target, pname, uint32(params) );

return
