function str = DirList(dirnm,qdispfiles,lim,pref)
% str = DirList(dirnm,qdispfiles,lim,pref)
% recursively lists directories en returns the whole shit
% as a string
% QDISPFILES is a boolean indicating whether files should also be listed
% (true, default)
% LIM is the maximum number of levels that will be listed (default unlimited (inf))
% PREF is a prefix for each node in the directorylist

% DN 2007
% DN and Sam Yeung 2008-07-28 Optionally displays files
% DN 2009-02-14  Made more intuitive


% input checking
if nargin<4
    pref = '';
end
if nargin<3
    lim = inf;
end
if nargin<2
    qdispfiles = true;
end

% init
str     = [];
fnms    = dir(dirnm);

% do the work
for p=1:length(fnms)
    if strcmp(fnms(p).name,'..') || strcmp(fnms(p).name,'.') % always returned by Matlab, never wanted
        continue;
    end

    if qdispfiles && ~isdir([dirnm filesep fnms(p).name])
        str = [str pref char(215) ' ' fnms(p).name char(10)];
    end
    if isdir([dirnm filesep fnms(p).name])
        str = [str pref '- ' fnms(p).name char(10)];
        if lim>0
            str = [str DirList([dirnm filesep fnms(p).name], qdispfiles, lim-1, [pref '  '])];
        end
    end
end