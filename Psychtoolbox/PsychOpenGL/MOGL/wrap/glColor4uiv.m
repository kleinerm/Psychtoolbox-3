function glColor4uiv( v )

% glColor4uiv  Interface to OpenGL function glColor4uiv
%
% usage:  glColor4uiv( v )
%
% C function:  void glColor4uiv(const GLuint* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor4uiv', uint32(v) );

return
