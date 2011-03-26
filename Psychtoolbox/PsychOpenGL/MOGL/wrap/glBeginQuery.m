function glBeginQuery( target, id )

% glBeginQuery  Interface to OpenGL function glBeginQuery
%
% usage:  glBeginQuery( target, id )
%
% C function:  void glBeginQuery(GLenum target, GLuint id)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBeginQuery', target, id );

return
