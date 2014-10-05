function data = glGetVariantFloatvEXT( id, value )

% glGetVariantFloatvEXT  Interface to OpenGL function glGetVariantFloatvEXT
%
% usage:  data = glGetVariantFloatvEXT( id, value )
%
% C function:  void glGetVariantFloatvEXT(GLuint id, GLenum value, GLfloat* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = single(0);

moglcore( 'glGetVariantFloatvEXT', id, value, data );

return
