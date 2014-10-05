function glTexParameterIuivEXT( target, pname, params )

% glTexParameterIuivEXT  Interface to OpenGL function glTexParameterIuivEXT
%
% usage:  glTexParameterIuivEXT( target, pname, params )
%
% C function:  void glTexParameterIuivEXT(GLenum target, GLenum pname, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameterIuivEXT', target, pname, uint32(params) );

return
