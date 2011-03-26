function glColor4ubv( v )

% glColor4ubv  Interface to OpenGL function glColor4ubv
%
% usage:  glColor4ubv( v )
%
% C function:  void glColor4ubv(const GLubyte* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor4ubv', uint8(v) );

return
