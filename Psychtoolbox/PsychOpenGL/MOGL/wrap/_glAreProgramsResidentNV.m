function [ r, residences ] = glAreProgramsResidentNV( n, programs )

% glAreProgramsResidentNV  Interface to OpenGL function glAreProgramsResidentNV
%
% usage:  [ r, residences ] = glAreProgramsResidentNV( n, programs )
%
% C function:  GLboolean glAreProgramsResidentNV(GLsizei n, const GLuint* programs, GLboolean* residences)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

residences = uint8(0);

r = moglcore( 'glAreProgramsResidentNV', n, uint32(programs), residences );

return
