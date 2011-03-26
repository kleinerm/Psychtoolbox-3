function glColor4usv( v )

% glColor4usv  Interface to OpenGL function glColor4usv
%
% usage:  glColor4usv( v )
%
% C function:  void glColor4usv(const GLushort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor4usv', uint16(v) );

return
