function glColor4ubVertex2fvSUN( c, v )

% glColor4ubVertex2fvSUN  Interface to OpenGL function glColor4ubVertex2fvSUN
%
% usage:  glColor4ubVertex2fvSUN( c, v )
%
% C function:  void glColor4ubVertex2fvSUN(const GLubyte* c, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glColor4ubVertex2fvSUN', uint8(c), single(v) );

return
