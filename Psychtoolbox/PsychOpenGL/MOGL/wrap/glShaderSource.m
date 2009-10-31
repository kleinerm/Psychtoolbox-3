function glShaderSource( shader, shadersource, debug )
% glShaderSource  Interface to glShaderSource
%
% usage:  glShaderSource(shader, shadersource, debug)
%
% This function differs from its C counterpart. You have to pass in the
% handle of the shader as returned by glCreateShader in 'shader'.
%
% You have to pass a single character string 'shadersource' that
% contains the ASCII text of the shaders source code. Each line in the
% string needs to be terminated by a '\n' aka ASCII code 10 aka newline.
% This string will be split up into the array of strings as expected by
% the C function glShaderSource.
%
% A simple way to read a shader from a standard text file is as follows:
% fid=fopen('MyShader.txt', 'r');
% shadersource=fread(fid);
% fclose(fid);
% glShaderSource(shader, shadersource);
%
% C function:  void glShaderSource(GLuint shader, int numOfStrings, const char **strings, int *lenOfStrings);

% 29-Mar-2006 -- created manually by MK.

% ---protected---

if nargin<2,
    error('invalid number of arguments');
end

% Special debug flag: If provided and set to non-zero, then glShaderSource is
% not executed, but only the parser inside moglcore is exercised and the result
% of parsing is output to the Matlab command window.
if nargin < 3
    debug = 0;
end;

shadersource = char(shadersource);
if size(shadersource,1)==1
    shadersource = transpose(shadersource);
end

% Replace all CR's by LF's:
repidx = find(double(shadersource) == 13);
shadersource(repidx) = char(10);

moglcore('glShaderSource', shader, transpose(shadersource), debug);

return
