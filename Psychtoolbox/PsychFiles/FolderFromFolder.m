function [fold,nfold] = FolderFromFolder(folder,mode)
% [fold,nfold] = FolderFromFolder(folder,mode)
%
% Returns struct with all directories in directory FOLDER.
% MODE specifies whether an error is displayed when no directories are
% found (default). If MODE is 'silent', only a message will will be
% displayed in the command window.

% 2007 IH        Wrote it.
% 2007 IH&DN     Various additions
% 2008-08-06 DN  All file properties now in output struct
% 2009-02-14 DN  Now returns all folders except '..' and '.', code
%                optimized
% 2010-07-02 DN  Fixed typo in warning when silent and no files found, got
%                rid of for loop

if nargin == 2 && strcmp(mode,'silent')
    silent = true;
else
    silent = false;
end

fold        = dir(folder);
fold        = fold([fold.isdir]);
% now skip '..' and '.', do not want to assume they are the first two
% elements returned
qremove     = ismember({fold.name},{'.','..'});
fold(qremove) = [];


nfold = length(fold);

if nfold==0
    if silent
        fprintf('FolderFromFolder: No folders found in: %s\n',folder);
        fold = [];
    elseif ~silent
        error('FolderFromFolder: No folders found in: %s',folder);
    end
end