function ThePath = DaqtoolboxConfigDir
% Syntax: AbsolutePath = DaqtoolboxConfigDir
%
% Purpose: Look for (create if necessary) folder containing preferences for Daq
%          toolbox;
%
% History: 1/28/08  mpr configured this was needed
%
% Function does not assume that user is using Psychophysics toolbox, but will
% subordinate Daqtoolbox if that assumption is correct.  That is, function will
% look for Daqtoolbox preferences in a folder containing Psychtoolbox
% preferences.  It will create its own folder iff it does not find one, and that
% folder will be in the Psychtoolbox preferences folder if that folder exists.

ThePath = which('DaqPrefsFolder.m');

if isempty(ThePath)
  % Couldn't resist the variable name, sorry...
  Psychopath=which('PsychPrefsFolder.m');
  if isempty(Psychopath)
    if IsOSX
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
                             'preferences folder in the current working directory: %s.\n'],pwd));
            StringStart = [pwd filesep];
          end
        else
          FoundHomeDir = 1;        
        end
        if FoundHomeDir
          [DirMade,DirMessage]=mkdir(HomeDir,'Application Data');
          if DirMade
            StringStart = [HomeDir filesep 'Application Data' filesep];
          else
            warning(sprintf('"Application Data" folder neither exists nor is createable;\nstoring preferences in home directory.'));
            StringStart = [HomeDir filesep];
          end
        end
      else
        StringStart = [StringStart filesep];
      end
    else % if IsOSX; else
      fprintf(['I do not know your operating system, so I don''t know where I should store\n' ...
              'Preferences.  I''m putting them in the current working directory:\n      %s.\n\n'],pwd);
      StringStart = [pwd filesep];
    end % if IsOSX; else
  else % if isempty(Psychopath)
    TheFileseps = find(Psychopath == filesep);
    StringStart = Psychopath(1:(TheFileseps(end)));
  end
  
  TheDir = [ StringStart 'Daqtoolbox'];
    
  TheMessage = ['%% This file exists only to let others know of its existence.  It was\n' ...
                '%% created by DaqtoolboxConfigDir.  It identifies the directory where\n' ...
                '%% configuration data for the Daq toolbox is stored.\n' ...
                '%% If you would like preferences to be stored some place else, move\n' ...
                '%% the folder containing this file to the location you want, and (if\n' ...
                '%% you have one), edit your startup file to recognize the new path.\n' ...
                '%% Your startup file should contain these lines (uncommented):\n\n' ...
                '\n%% %% The next few lines were automatically written by DaqtoolboxConfigDir.\n' ...
                '%% %% Do NOT edit them unless you know what you are doing!\n\n' ...
                '%% CurrentPath = path;\n' ...
                '%% if isempty(strfind(CurrentPath,''[PATH_TO_THIS_FOLDER]''))\n' ...
                '%%   addpath(''[PATH_TO_THIS_FOLDER]'',''-END'');\n' ...
                '%% end;\n\n' ...
                '%% %% Okay to edit after this.\n\n' ...
                '%% Replace PATH_TO_THIS_FOLDER with the location where you moved this\n' ...
                '%% folder, and your preferences should always be found when you start Matlab.\n'];
              

  if exist(TheDir,'dir')
    if exist([TheDir filesep 'DaqPrefsFolder.m'],'file')
      ThePath = TheDir;
    else
      fid=fopen([TheDir filesep 'DaqPrefsFolder.m'],'a');
      if fid < 0
        error(sprintf('I could not create a configuration file in %s.  Are your file permissions okay?',TheDir));
      end
      fprintf(fid,TheMessage);
      fclose(fid); 
      ThePath=TheDir;
    end
  else % if exist(TheDir,'dir')
    if IsLinux
      [DirMade, DirMessage] = mkdir(StringStart(1:(end-1)),'.Daqtoolbox');
    else      
      [DirMade, DirMessage] = mkdir(StringStart,'Daqtoolbox');
    end
    if DirMade
      TheDir = [StringStart 'Daqtoolbox' filesep];
      fid=fopen([TheDir 'DaqPrefsFolder.m'],'a');
      if fid < 0
        error(sprintf('I could not create a configuration file in %s.  Are your file permissions okay?',TheDir));
      end
      fprintf(fid,TheMessage);
      fclose(fid);
      ThePath=TheDir;
    else % if DirMade
      error(sprintf('I could not create a folder to store your preferences in\n\n%s\n\nWhat are the permissions on that folder?',StringStart));
    end % if DirMade; else
  end % if exist(TheDir,'dir')
else % if isempty(ThePath)
  FileSeps=find(ThePath==filesep);
  ThePath=ThePath(1:(FileSeps(end)-1));
end % if isempty(ThePath); else

CurrentPath = path;
if isempty(strfind(CurrentPath,ThePath))
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
        elseif ~isempty(strfind(NextLine,'DaqtoolboxConfigDir'))
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
            ClearCurrentPathLine = fgetl(fid);
            BlankLine2 = fgetl(fid);
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
                             'clear CurrentPath;\n\n' ...
                             '%% Okay to edit after this.\n'],ThePath);
                if ~strcmp(EndLine,'end;') || isempty(BlankLine) || ~strcmp(OkayToEditLine,'% Okay to edit after this.') || ...
                           ~strcmp(ClearCurrentPathLine,'clear CurrentPath;') || isempty(BlankLine2)
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
                  fseek(fid,DistFromStart,-1);
                  NewString = char(' '*ones(1,LengthOfFile-DistFromStart+1));
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
        end % if ~ischar(NextLine); elseif ~isempty(strfind(NextLine,'DaqtoolboxConfigDir'))
      end % if ReadingFile
      
      if ReachedEnd
        fprintf(fid,['\n\n%% The next few lines were automatically written by DaqtoolboxConfigDir.\n' ...
                     '%% Do NOT edit them unless you know what you are doing!\n\n' ...
                     'CurrentPath = path;\n' ...
                     'if isempty(strfind(CurrentPath,''%s''))\n' ...
                     '  addpath(''%s'',''-END'');\n' ...
                     'end;\n\n' ...
                     'clear CurrentPath;\n\n' ...
                     '%% Okay to edit after this.\n\n'],ThePath,ThePath);
      end
      fclose(fid);
    end % if fid > 0 % successfuly opened startup file
  end % if isempty(TheStartup); else
end % if isempty(strfind(CurrentPath,ThePath))
return;

