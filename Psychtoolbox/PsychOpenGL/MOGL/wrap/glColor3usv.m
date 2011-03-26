function glColor3usv( v )

% glColor3usv  Interface to OpenGL function glColor3usv
%
% usage:  glColor3usv( v )
%
% C function:  void glColor3usv(const GLushort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor3usv', uint16(v) );

return
