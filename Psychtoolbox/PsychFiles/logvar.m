function logvar(var,name,dirnm)
% logvar(var,name,dirnm)
%
% Turns a variable VAR into a string that evaluates back to the original
% variable, e.g. when feeding it to eval()
% NAME is the name of the variable that will be used in this string
% String will be saved in a text file in the directory DIRNM with as
% filename the name of the variable NAME and the time at which the file is
% written.
%
% DN 2008

fs = filesep;

str         = var2str(var,name);
logtijd     = clock;
logfilename = [dirnm fs name ' log ' strpad(logtijd(1),4,0) '-' strpad(logtijd(2),2,0) '-' strpad(logtijd(3),2,0) ' ' strpad(logtijd(4),2,0) strpad(logtijd(5),2,0) '.txt'];
fid         = fopen(logfilename, 'wt');
fprintf(fid,'%s',str);
fclose(fid);