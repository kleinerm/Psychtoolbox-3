function params = glGetObjectBufferivATI( buffer, pname )

% glGetObjectBufferivATI  Interface to OpenGL function glGetObjectBufferivATI
%
% usage:  params = glGetObjectBufferivATI( buffer, pname )
%
% C function:  void glGetObjectBufferivATI(GLuint buffer, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetObjectBufferivATI', buffer, pname, params );

return
