function glEnablei( target, index )

% glEnablei  Interface to OpenGL function glEnablei
%
% usage:  glEnablei( target, index )
%
% C function:  void glEnablei(GLenum target, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEnablei', target, index );

return
