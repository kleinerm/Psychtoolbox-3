function glEvalPoint1( i )

% glEvalPoint1  Interface to OpenGL function glEvalPoint1
%
% usage:  glEvalPoint1( i )
%
% C function:  void glEvalPoint1(GLint i)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEvalPoint1', i );

return
