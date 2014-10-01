function names = glGenNamesAMD( identifier, num )

% glGenNamesAMD  Interface to OpenGL function glGenNamesAMD
%
% usage:  names = glGenNamesAMD( identifier, num )
%
% C function:  void glGenNamesAMD(GLenum identifier, GLuint num, GLuint* names)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

names = uint32(0);

moglcore( 'glGenNamesAMD', identifier, num, names );

return
