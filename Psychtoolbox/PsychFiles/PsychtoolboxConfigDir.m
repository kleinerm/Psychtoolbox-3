function ThePath=PsychtoolboxConfigDir
% Syntax: path=PsychtoolboxConfigDir
%
% Purpose: look for a folder for storing Psychtoolbox preferences; create if
%          necessary.
%
% History: 1/23/08    mpr configured it was about time to write this
%          3/7/08     mpr streamlined this
%          3/8/08     mk  A bit more of streamlining - Don't write the
%                         PsychPrefsfolder.m file anymore.

persistent PTBPrefPath %#ok<REDEF>

if ~isempty(PTBPrefPath) %#ok<NODEF>
  if exist(PTBPrefPath,'dir') %#ok<NODEF>
    ThePath=PTBPrefPath;
    return;
  end
end

if IsOSX
  % Did this instead of '~/' because the which command above and the addpath
  % commands below will expand '~/' to a full path; echoing the HOME
  % environment variable was the first way I found to get said full path so
  % that strings will match when they should
  [ErrMsg,HomeDir] = unix('echo $HOME');
  % end-1 to trim trailing carriage return
  StringStart = [HomeDir(1:(end-1)) '/Library/Preferences/'];
elseif IsLinux
  [ErrMsg,HomeDir] = unix('echo $HOME');    
  % end-1 to trim trailing carriage return
  StringStart = [HomeDir(1:(end-1)) '/.'];
elseif IsWindows
  [ErrMsg,StringStart] = dos('echo %AppData%');
  % end-1 to trim trailing carriage return
  StringStart = StringStart(1:(end-1));
  if strcmp(StringStart,'%AppData%')
    FoundHomeDir = 0;
    [ErrMsg,HomeDir] = dos('echo %UserProfile%');
    HomeDir = HomeDir(1:(end-1));
    if strcmp(HomeDir,'%UserProfile%')
      HomeDir = uigetdir('','Please find your home folder for me');
      if ischar(HomeDir)
        FoundHomeDir = 1;
      else
        warning(sprintf(['I could not find your home directory or understand your input so I am storing\n' ...
                         'preferences folder in the current working directory: %s.\n'],pwd)); %#ok<SPWRN>
        StringStart = [pwd filesep];
      end
    else
      FoundHomeDir = 1;        
    end
    if FoundHomeDir
      [DirMade,DirMessage]=mkdir(HomeDir,'Application Data'); %#ok<NASGU>
      if DirMade
        StringStart = [HomeDir filesep 'Application Data' filesep];
      else
        warning(sprintf('"Application Data" folder neither exists nor is createable;\nstoring preferences in home directory.')); %#ok<WNTAG,SPWRN>
        StringStart = [HomeDir filesep];
      end
    end
  else
    StringStart = [StringStart filesep];
  end
else
  fprintf(['I do not know your operating system, so I don''t know where I should store\n' ...
          'Preferences.  I''m putting them in the current working directory:\n      %s.\n\n'],pwd);
  StringStart = [pwd filesep];
end

TheDir = [StringStart 'Psychtoolbox'];

if exist(TheDir,'dir')
  ThePath = TheDir; %#ok<NASGU>
else
  % Could just use:
  % [DirMade, DirMessage] = mkdir(TheDir);
  % but I've had problems (in OS 9) having mkdir fail when string passed is too
  % long.  I don't know if that has been fixed in other OS's, so this is safe:
  if IsLinux
    [DirMade, DirMessage] = mkdir(StringStart(1:(end-1)),'.Psychtoolbox'); %#ok<NASGU>
  else      
    [DirMade, DirMessage] = mkdir(StringStart,'Psychtoolbox'); %#ok<NASGU>
  end

  if DirMade
    TheDir = [StringStart 'Psychtoolbox'];
    ThePath=TheDir; %#ok<NASGU>
  else % if exist(TheDir,'dir')
    error(sprintf('I could not create a folder to store your preferences in\n\n%s\n\nWhat are the permissions on that folder?',StringStart)); %#ok<SPERR>
  end % if exist(TheDir,'dir'); else
end

ThePath = [ThePath filesep];
PTBPrefPath = ThePath;
return;
