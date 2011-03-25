function cwrite( fid, funcp, C )

% CWRITE  Write a C interface function
% 
% cwrite( fid, funcp, C )

% 24-Jan-2006 -- created;  adapted from code in autocode.m (RFM)
% 21-Mar-2006 -- Extended with checking for unsupported glXXX functions. (MK)
% 24-Mar-2011 -- Modified; Allow 64-bit safe wrapping of encoded memory pointers. (MK)

fprintf(fid,'void %s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {\n\n\t',C.interfacename);
fprintf(fid,'if (NULL == %s) mogl_glunsupported("%s");\n\t', funcp.fname, funcp.fname);
if C.hasptrreturn
    % Return value is an encoded memory pointer:
    fprintf(fid,'%s%s(%s));\n',C.arg_out{1},funcp.fname,strrep(commalist(C.arg_in{:}),', ',sprintf(',\n\t\t')));
else
    % Return value (if any) is a scalar or something else innocent:
    fprintf(fid,'%s%s(%s);\n',C.arg_out{1},funcp.fname,strrep(commalist(C.arg_in{:}),', ',sprintf(',\n\t\t')));
end
fprintf(fid,'\n}\n\n');

return
