function params = glGetObjectParameterivAPPLE( objectType, name, pname )

% glGetObjectParameterivAPPLE  Interface to OpenGL function glGetObjectParameterivAPPLE
%
% usage:  params = glGetObjectParameterivAPPLE( objectType, name, pname )
%
% C function:  void glGetObjectParameterivAPPLE(GLenum objectType, GLuint name, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetObjectParameterivAPPLE', objectType, name, pname, params );

return
