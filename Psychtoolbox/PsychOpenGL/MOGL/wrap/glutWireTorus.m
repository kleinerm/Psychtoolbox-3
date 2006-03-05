function glutWireTorus( innerRadius, outerRadius, sides, rings )

% glutWireTorus  Interface to OpenGL function glutWireTorus
%
% usage:  glutWireTorus( innerRadius, outerRadius, sides, rings )
%
% C function:  void glutWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glutWireTorus', innerRadius, outerRadius, sides, rings );

return
