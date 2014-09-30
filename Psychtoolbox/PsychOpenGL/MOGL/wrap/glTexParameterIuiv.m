function glTexParameterIuiv( target, pname, params )

% glTexParameterIuiv  Interface to OpenGL function glTexParameterIuiv
%
% usage:  glTexParameterIuiv( target, pname, params )
%
% C function:  void glTexParameterIuiv(GLenum target, GLenum pname, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameterIuiv', target, pname, uint32(params) );

return
