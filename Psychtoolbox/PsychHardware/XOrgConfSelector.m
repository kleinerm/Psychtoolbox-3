function XOrgConfSelector(sdir)
% XOrgConfSelector([sdir]) - Select a X11 configuration file to apply.
%
% By default configuration files are chosen from a PsychtoolboxConfigDir
% subfolder, where XOrgConfCreator creates and stores them by default.
%
% You can override the choice of the directory for config files by
% providing the optional 'sdir' argument with the path to a different
% folder.
%
% The chosen configuration file is copied into the systems
% /etc/X11/xorg.conf.d/ folder for the X-Server to pick it up and apply it
% during the next logout->login cycle or restart.
%

% History:
% 04-Nov-2015  mk  Created.

  if nargin < 1 || isempty(sdir)
    sdir = PsychtoolboxConfigDir ('XorgConfs');
  end

  conffiles = {};
  candfiles = dir(sdir);
  for i=1:length(candfiles)
    if ~candfiles(i).isdir
      conffiles{end+1} = candfiles(i).name;
    end
  end

  fprintf('You can choose from the following X11 configuration files:\n\n');
  for i=1:length(conffiles)
    fprintf('%i) %s\n', i, conffiles{i});
  end
  fprintf('\n');
  answer = [];
  while isempty(answer) || ~ismember(answer, 0:length(conffiles))
    answer = input('Which one should i use? (Choose 0 for remove the current active file): ');
  end
  fprintf('\n\n');

  updated = 0;

  if answer == 0
    % Remove existing file:
    if exist('/etc/X11/xorg.conf.d/90-ptbxorg.conf')
      delete('/etc/X11/xorg.conf.d/90-ptbxorg.conf');
      fprintf('Removed the old existing xorg.conf file from the X11 config folder. under\n');
      updated = 1;
    else
      fprintf('There isn''t any old configuration file to remove in the X11 config folder under\n');
    end
  else
    % Copy selected file into the X11 config directory:
    copyfile([sdir conffiles{answer}], '/etc/X11/xorg.conf.d/90-ptbxorg.conf');
    fprintf('Copied the xorg.conf file into the X11 config folder. under\n');
    updated = 1;
  end

  fprintf('/etc/X11/xorg.conf.d/90-ptbxorg.conf\n\n');
  if updated
    fprintf('Now please logout and login again for the configuration to take effect.\n\n');
  end
end
