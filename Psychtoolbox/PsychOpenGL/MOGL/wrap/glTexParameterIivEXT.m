function glTexParameterIivEXT( target, pname, params )

% glTexParameterIivEXT  Interface to OpenGL function glTexParameterIivEXT
%
% usage:  glTexParameterIivEXT( target, pname, params )
%
% C function:  void glTexParameterIivEXT(GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameterIivEXT', target, pname, int32(params) );

return
