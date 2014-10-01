function glDisablei( target, index )

% glDisablei  Interface to OpenGL function glDisablei
%
% usage:  glDisablei( target, index )
%
% C function:  void glDisablei(GLenum target, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDisablei', target, index );

return
