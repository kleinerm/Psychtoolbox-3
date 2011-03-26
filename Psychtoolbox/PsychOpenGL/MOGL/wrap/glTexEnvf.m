function glTexEnvf( target, pname, param )

% glTexEnvf  Interface to OpenGL function glTexEnvf
%
% usage:  glTexEnvf( target, pname, param )
%
% C function:  void glTexEnvf(GLenum target, GLenum pname, GLfloat param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexEnvf', target, pname, param );

return
