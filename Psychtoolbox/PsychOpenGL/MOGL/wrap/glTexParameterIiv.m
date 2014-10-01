function glTexParameterIiv( target, pname, params )

% glTexParameterIiv  Interface to OpenGL function glTexParameterIiv
%
% usage:  glTexParameterIiv( target, pname, params )
%
% C function:  void glTexParameterIiv(GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameterIiv', target, pname, int32(params) );

return
