function glLinkProgram( program )

% glLinkProgram  Interface to OpenGL function glLinkProgram
%
% usage:  glLinkProgram( program )
%
% C function:  void glLinkProgram(GLuint program)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLinkProgram', program );

return
