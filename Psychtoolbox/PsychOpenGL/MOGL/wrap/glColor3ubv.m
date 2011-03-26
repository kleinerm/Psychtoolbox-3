function glColor3ubv( v )

% glColor3ubv  Interface to OpenGL function glColor3ubv
%
% usage:  glColor3ubv( v )
%
% C function:  void glColor3ubv(const GLubyte* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor3ubv', uint8(v) );

return
