function glTextureParameteri( texture, pname, param )

% glTextureParameteri  Interface to OpenGL function glTextureParameteri
%
% usage:  glTextureParameteri( texture, pname, param )
%
% C function:  void glTextureParameteri(GLuint texture, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameteri', texture, pname, param );

return
