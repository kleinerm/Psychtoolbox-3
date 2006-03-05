function glPassThrough( token )

% glPassThrough  Interface to OpenGL function glPassThrough
%
% usage:  glPassThrough( token )
%
% C function:  void glPassThrough(GLfloat token)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPassThrough', token );

return
