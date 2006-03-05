function glFogi( pname, param )

% glFogi  Interface to OpenGL function glFogi
%
% usage:  glFogi( pname, param )
%
% C function:  void glFogi(GLenum pname, GLint param)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFogi', pname, param );

return
