function ThePath=PsychtoolboxConfigDir
% Syntax: path=PsychtoolboxConfigDir
%
% Purpose: look for a folder for storing Psychtoolbox preferences; create if
%          necessary.
%
% History: 1/23/08    mpr configured it was about time to write this

ThePath=which('PsychPrefsFolder.m');

if isempty(ThePath)
  TheMessage = ['%% This file exists only to let others know of its existence.  It was\n' ...
                        '%% created by PsychtoolboxConfigDir.  It identifies the directory where\n' ...
                        '%% configuration data for the Psychophysics toolbox is stored.\n'];
  if IsOSX
    % Did this instead of '~/' because the which command above and the addpath
    % commands below will expand that to a full path; echoing the HOME
    % environment variable was the first way I found to get said full path so
    % that strings will match when they should
    [ErrMsg,HomeDir] = unix('echo $HOME');
    StringStart = [HomeDir '/Library/Preferences/'];
  elseif IsLinux
    [ErrMsg,HomeDir] = unix('echo $HOME');    
    StringStart = [HomeDir '/.'];
  elseif IsWindows
    StartupPath = which('startup');
    if isempty(StartupPath)
      StringStart = FindWindozeDir;
      % I'm 99% certain FindWindozeDir will never come back empty, but just
      % in case...
      if isempty(StringStart)
        warning('I could not find any reasonable places for Preferences folder so I am using current directory');
        StringStart = [pwd filesep];
      end
    else % if isempty(StartupPath);
      StringStart = StartupPath(1:(strfind(StartupPath,'startup')-1));
    end
  else
    fprintf(['I do not know your operating system, so I don''t know where I should store\n' ...
            'Preferences.  I''m putting them in the current working directory:\n      %s.\n\n'],pwd);
    StringStart = [pwd filesep];
  end
  
  TheDir = [StringStart 'Psychtoolbox'];

  if exist(TheDir,'dir')
    if exist([TheDir filesep 'PsychPrefsFolder.m'],'file')
      ThePath = [TheDir filesep];
    else
      fid=fopen([TheDir filesep 'PsychPrefsFolder.m'],'a');
      if fid < 0
        error(sprintf('I could not create a configuration file in %s.  Are your file permissions okay?',TheDir));
      end
      fprintf(fid,TheMessage);
      fclose(fid); 
      ThePath=[TheDir filesep];
    end
  else % if exist(TheDir,'dir')
    if IsLinux
      [DirMade, DirMessage] = mkdir(StringStart(1:(end-1)),'.Psychtoolbox');
    else      
      [DirMade, DirMessage] = mkdir(StringStart,'Psychtoolbox');
    end
    if DirMade
      TheDir = [StringStart 'Psychtoolbox' filesep];
      fid=fopen([TheDir 'PsychPrefsFolder.m'],'a');
      if fid < 0
        error(sprintf('I could not create a configuration file in %s.  Are your file permissions okay?',TheDir));
      end
      fprintf(fid,TheMessage);
      fclose(fid);
      ThePath=[TheDir filesep];
    else % if DirMade
      error(sprintf('I could not create a folder to store your preferences in %s. \nWhat are the permissions on that folder?',StringStart));
    end % if DirMade; else
  end % if exist(TheDir,'dir')
else % if isempty(ThePath)
  FileSeps=find(ThePath==filesep);
  ThePath=ThePath(1:FileSeps(end));
end % if isempty(ThePath); else

% This should always be true, but just in case...
if ThePath(end) == filesep
  % need to trim off ending file separator because Matlab doesn't retain it in
  % path
  CompPath = ThePath(1:(end-1));
else
  CompPath = ThePath;
end

CurrentPath = path;
if isempty(strfind(CurrentPath,CompPath))
  addpath(ThePath,'-END');
  TheStartup = which('startup.m');
  if isempty(TheStartup)
    warning('I could not find a startup file, so preference location will not be stored for future Matlab runs.');
  else
    fid = fopen(TheStartup,'r+t');
    if fid > 0
      % Just to be safe
      frewind(fid);
      ReadingFile=1;
      ReachedEnd = 0;
      while ReadingFile
        NextLine = fgetl(fid);
        if ~ischar(NextLine)
          ReachedEnd = 1;
          ReadingFile = 0;
        elseif ~isempty(strfind(NextLine,'PsychtoolboxConfigDir'))
          NewLines = 0;
          while NewLines < 10
            NextLine = fgetl(fid);
            if strcmp(NextLine,'CurrentPath = path;')
              break;
            else
              NewLines = NewLines+1;
            end
          end
          CorruptedStartup = 0;
          if NewLines > 9
            CorruptedStartup = 1;
          else
            PathCheckLinePosition = ftell(fid);
            PathCheckLine = fgetl(fid);
            AddPathLine = fgetl(fid);
            EndLine = fgetl(fid);
            BlankLine = fgetl(fid);
            OkayToEditline = fgetl(fid);
            RestOfFile = fread(fid);
            LastPath = sscanf(PathCheckLine,'if isempty(strfind(CurrentPath,''%s'',''-END'');');
            if isempty(LastPath)
              CorruptedStartup = 1;
            elseif ~strcmp(ThePath,LastPath)
              fseek(fid,PathCheckLinePosition,-1);
              fprintf(fid,'if isempty(strfind(CurrentPath,''%s''))\n',ThePath);
              LastPath = sscanf(AddPathLine,'  addpath(''%s');
              EndOfTheLine = strfind(LastPath,''',''-END'');');
              LastPath = LastPath(1:(EndOfTheLine-1));
              if ~strcmp(ThePath,LastPath)
                warning(sprintf(['There appears to have been a mismatch in previously specified path information.\n' ...
                         'Future runs should find your current preferences, but I don''t know what was going\n' ...
                         'on before.  You might want to investigate your startup file.\n']));
                fprintf(fid,['  addpath(''%s'',''-END'');\n' ...
                             'end;\n\n' ...
                             '%% Okay to edit after this.\n'],ThePath);
                if ~strcmp(EndLine,'end;') || isempty(BlankLine) || ~strcmp(OkayToEditLine,'% Okay to edit after this.')
                  warning(sprintf(['It looks like someone edited your startup file against recommendations.  You should \n' ...
                                  'look at the file as it now may need fixing.\n']));
                end
                fwrite(fid,RestOfFile);
                % found that if newer path is shorter than older path, then part
                % of old file remains untouched by print and write operations
                % above.  Could not find a way to write an EOF character, so I
                % overwrite with spaces if need be.
                if ~feof(fid)
                  DistFromStart = ftell(fid);
                  fseek(fid,0,1);
                  LengthOfFile = ftell(fid);
                  fseek(DistFromStart,0,0);
                  NewString = ' '*ones(1,LengthOfFile-DistFromStart+1);
                  fprintf(fid,'%s',NewString');
                end
              end
            end % if isempty(LastPath); elseif ~strcmp(ThePath,LastPath)
          end % if NewLines > 9
          if CorruptedStartup
            warning(sprintf(['Someone seems to have corrupted your startup file.  In future runs,\n' ...
                             'your previous (or more likely current) configuration files may not be used.\n' ...
                             'You should investigate and edit your startup file accordingly.']));
          end
          ReadingFile = 0;
        end % if ~ischar(NextLine); elseif ~isempty(strfind(NextLine,'PsychtoolboxConfigDir'))
      end % if ReadingFile
      
      if ReachedEnd
        fprintf(fid,['\n\n%% The next few lines were automatically written by PsychtoolboxConfigDir.\n' ...
                     '%% Do NOT edit them unless you know what you are doing!\n\n' ...
                     'CurrentPath = path;\n' ...
                     'if isempty(strfind(CurrentPath,''%s''))\n' ...
                     '  addpath(''%s'',''-END'');\n' ...
                     'end;\n\n' ...
                     '%% Okay to edit after this.\n\n'],ThePath,ThePath);
      end
      fclose(fid);
    end % if fid > 0 % successfuly opened startup file
  end % if isempty(TheStartup); else
end % if isempty(strfind(CurrentPath,ThePath))
return;

function StringStart=FindWindozeDir

MyHomePath = FindMyHomePath;
if isempty(MyHomePath)
  ProfPath = [];
  if exist([DiskRoot 'Users'],'dir')
    ProfPath = [DiskRoot 'Users'];
  elseif exist([DiskRoot 'Documents and Settings'],'dir')
    ProfPath = [DiskRoot 'Documents and Settings'];
  elseif exist([DiskRoot 'WINNT' filesep 'Profiles'],'dir')
    ProfPath = [DiskRoot 'WINNT' filesep 'Profiles'];
  elseif exist([DiskRoot 'My Documents'],'dir')
    ProfPath = [DiskRoot 'My Documents'];
  end
  MyHomeDir = uigetdir(ProfPath,'Open your home folder');
  if ischar(MyHomeDir)
    StringStart = CrawlDownHomeDir(MyHomeDir);
  else
    warning(sprintf(['I could not find your home directory or understand your input so I am storing\n' ...
                     'preferences folder in the current working directory: %s.\n'],pwd));
    [DirMade,DirMessage] = mkdir(pwd,'Matlab');
    if DirMade
      fprintf('At least I was able to find or create a Matlab sub-folder.\n\n');
      StringStart = [pwd filesep 'Matlab' filesep];
    else
      fprintf('I could not even create a Matlab sub-folder!\n\n');
      StringStart = [pwd filesep];
    end
  end
else
  StringStart = CrawlDownHomeDir(MyHomeDir);
end
return;


function TheShortestDir = ExtractShortestDir(InclusiveSet,ShortPattern)

AllPossStarts = regexpi(InclusiveSet,ShortPattern);
% Only called if IsWindows, but I briefly had a crazy idea that I should check
% anyways, and now I'd rather leave something redundant than reduce portability
if IsWindows
  FolderSepPoss = find(InclusiveSet == ';');
else
  FolderSepPoss = find(InclusiveSet == ':');
end
  
for k=1:length(AllPossStarts)
  FirstSeps = FolderSepPoss(find(FolderSepPoss < AllPossStarts(k)));
  if isempty(FirstSeps)
    TheStart = 1;
  else
    TheStart = FirstSeps(end)+1;
  end
  LastSeps = FolderSepPoss(find(FolderSepPoss > AllPossStarts(k)));
  if isempty(LastSeps)
    TheFinish = length(InclusiveSet);
  else
    TheFinish = LastSeps(1)-1;
  end
  AllTheDirs{k} = InclusiveSet(TheStart:TheFinish);
  TheLengths(k) = length(AllTheDirs{k});
end

[UselessMin,TheShortestInd] = min(TheLengths);

TheShortestDir = AllTheDirs{TheShortestInd};

return;

function MatlabDir = CrawlDownHomeDir(MyHomePath)

% The only system I've actually tested is running Windows NT and has "My
% Documents" inside the home directory, so I'll look for that, but won't enforce
% it if that subfolder doesn't already exist
MatlabDir = [MyHomePath filesep];
if exist([MatlabDir 'My Documents'],'dir')
  MatlabDir = [MatlabDir 'My Documents' filesep];
end

% really want to put things in a Matlab folder
[DirExists,DirMessage] = mkdir(MatlabDir,'Matlab');
if DirExists
  MatlabDir = [MatlabDir 'Matlab' filesep];
else
  warning(sprintf('I could not create Matlab directory in %s',MatlabDir));
end

return;


function MyHomePath = FindMyHomePath

[TheStatus,TheResult] = dos('echo %UserProfile%');
if ~strcmp(TheResult,'%UserProfile%')
  MyHomePath = TheResult;
  return;
end

[TheStatus,TheResult] = dos('echo %user%');
if ~strcmp(TheResult,'%user%')
  UserName = TheResult;
end

% Could also try using %OS%, %UserDomain%, but I'm not sure these approaches
% would be as reliable as the approaches currently utilized, especially since
% behavior of Windows NT depends on when it was produced

CurrentPath = path;

if exist('UserName','var')
  UserNameStarts = regexpi(CurrentPath,UserName);
  if isempty(UserNameStarts)
    if exist([DiskRoot 'Users' filesep UserName],'dir')
      MyHomePath = [DiskRoot 'Users' filesep UserName];
    elseif exist([DiskRoot 'Documents and Settings' filesep UserName],'dir')
      MyHomePath = [DiskRoot 'Documents and Settings' filesep UserName];
    elseif exist([DiskRoot 'WINNT' filesep 'Profiles' filesep UserName],'dir')
      MyHomePath = [DiskRoot 'WINNT' filesep 'Profiles' filesep UserName];
    elseif exist([DiskRoot 'My Documents' filesep UserName],'dir')
      MyHomePath = [DiskRoot 'My Documents' filesep UserName];
    else
      MyHomePath = [];
    end
  else
    MyHomePath = ExtractShortestDir(CurrentPath,UserName);
  end
else
  MyHomePath = [];
  if exist([DiskRoot 'Users'],'dir')
    ProfPath = [DiskRoot 'Users' filesep];
  elseif exist([DiskRoot 'WINNT' filesep 'Profiles'],'dir')
    ProfPath = [DiskRoot 'WINNT' filesep 'Profiles' filesep];
  elseif exist([DiskRoot 'Documents and Settings'],'dir')
    ProfPath = [DiskRoot 'Documents and Settings' filesep];
  elseif exist([DiskRoot 'My Documents'],'dir')
    ProfPath = [DiskRoot 'My Documents' filesep];
  end
  if exist('ProfPath','var')
    PossHomeStarts = regexpi(CurrentPath,ProfPath);
    LPP = length(ProfPath);
    if ~isempty(PossHomeStarts)
      AllPathSeps = find(CurrentPath == ';');
      AllFolderSeps = find(CurrentPath == filesep);
      for k=1:length(PossHomeStarts)
        PrePathSeps = AllPathSeps(find(AllPathSeps < PossHomeStarts(k)));
        if isempty(PrePathSeps)
          ThisPathStart = 1;
        else
          ThisPathStart = PrePathSeps(end)+1;
        end
        PostPathSeps = AllPathSeps(find(AllPathSeps > PossHomeStarts(k)));
        if isempty(PostPathSeps)
          ThisPathEnd = length(CurrentPath);
        else
          ThisPathEnd = PostPathSeps(1)-1;
        end
        ThisPath = CurrentPath(ThisPathStart:ThisPathEnd);
        LocalHomeStart = regexpi(ThisPath,ProfPath);
        if LocalHomeStart < length(ThisPath)-(LPP-1)
          if ThisPath(LocalHomeStart+LPP) == filesep
            if length(ThisPath) > LocalHomeStart+LPP
              PresumedUserName = Thispath((LPP+1):end);
              FileSepLocs = find(PresumedUserName == filesep);
              if ~isempty(FileSepLocs)
                PresumedUserName = PresumedUserName(1:(FileSepLocs(1)-1));
              end
              MyHomePath = ThisPath(1:(LocalHomeStart+LPP+length(PresumedUserName)));
              fprintf('It looks like your username is %s.  I hope that''s right!\n\n',PresumedUserName);
              return;
            end
          end
        end
      end
    end
  end    
end

return;

