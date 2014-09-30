function glBindAttribLocationARB( programObj, index, name )

% glBindAttribLocationARB  Interface to OpenGL function glBindAttribLocationARB
%
% usage:  glBindAttribLocationARB( programObj, index, name )
%
% C function:  void glBindAttribLocationARB(GLhandleARB programObj, GLuint index, const GLcharARB* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindAttribLocationARB', programObj, index, uint8(name) );

return
