function string = glGetProgramStringARB( target, pname )

% glGetProgramStringARB  Interface to OpenGL function glGetProgramStringARB
%
% usage:  string = glGetProgramStringARB( target, pname )
%
% C function:  void glGetProgramStringARB(GLenum target, GLenum pname, void* string)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

string = (0);

moglcore( 'glGetProgramStringARB', target, pname, string );

return
