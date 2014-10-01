function params = glGetFloati_vEXT( pname, index )

% glGetFloati_vEXT  Interface to OpenGL function glGetFloati_vEXT
%
% usage:  params = glGetFloati_vEXT( pname, index )
%
% C function:  void glGetFloati_vEXT(GLenum pname, GLuint index, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetFloati_vEXT', pname, index, params );

return
