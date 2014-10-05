function string = glGetNamedProgramStringEXT( program, target, pname )

% glGetNamedProgramStringEXT  Interface to OpenGL function glGetNamedProgramStringEXT
%
% usage:  string = glGetNamedProgramStringEXT( program, target, pname )
%
% C function:  void glGetNamedProgramStringEXT(GLuint program, GLenum target, GLenum pname, void* string)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

string = (0);

moglcore( 'glGetNamedProgramStringEXT', program, target, pname, string );

return
