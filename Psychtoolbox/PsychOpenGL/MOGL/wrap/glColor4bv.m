function glColor4bv( v )

% glColor4bv  Interface to OpenGL function glColor4bv
%
% usage:  glColor4bv( v )
%
% C function:  void glColor4bv(const GLbyte* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor4bv', int8(v) );

return
