function glMultiTexParameterIivEXT( texunit, target, pname, params )

% glMultiTexParameterIivEXT  Interface to OpenGL function glMultiTexParameterIivEXT
%
% usage:  glMultiTexParameterIivEXT( texunit, target, pname, params )
%
% C function:  void glMultiTexParameterIivEXT(GLenum texunit, GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexParameterIivEXT', texunit, target, pname, int32(params) );

return
