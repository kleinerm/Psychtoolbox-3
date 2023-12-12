function usedAnswers = PsychLinuxConfiguration(answers, dontshowad)
% PsychLinuxConfiguration([answers]) -- Optimize setup of Linux system.
%
% This script modifies system settings and configuration files
% to optimize a Linux system for use with Psychtoolbox.
%
% Currently it modifies files to allow to run Octave or Matlab
% as regular non-root user, ie. without need for root login or
% the "sudo" command. It does so by changing file permissions
% and resource usage limits to allow a regular user application
% to switch to realtime scheduling, lock its memory, and to
% access special purpose hardware like GPUs, Bits+, Datapixx and
% other research equipment.
%
% Realtime optimizations are achieved by extending the
% /etc/security/limits.conf file with entries that allow
% members of the Unix user group "psychtoolbox" to lock
% application memory into physical RAM, eliminating/minimizing
% interference from the VM subsystem, and to run with realtime
% priorities up to level 50. The group "psychtoolbox" is created
% if it does not already exist.
%
% If the target system has a /etc/security/limits.d/ directory,
% then a separate rule file is stored to that directory to
% achieve the change without messing around with the limits.conf
% file.
%
% root-less hardware access is achieved by copying a special
% psychtoolbox.rules file into the /etc/udev/rules.d/ directory.
% This udev rules file contains rules to auto-detect certain
% hardware at bootup or when the hw is hot-plugged and to
% reconfigure this hw or access permission for root-less access
% by Psychtoolbox, and for optimal performance for the kind
% of typical PTB use cases.
%
% The script also creates a /etc/X11/xorg.conf.d/ directory for
% xorg.conf configuration files which are writable by members
% of the 'psychtoolbox' group to allow easy reconfiguration of
% the X11 display system for running experiment sessions.
%
% Additionally it configures the gpu's to allow for display color
% depths of more than 8 bpc. Furthermore, it configures a potentially
% installed gamemoded daemon to further optimize cpu and graphics
% performance for use with Psychtoolbox.
%
% The script calls into the shell via "sudo" to achieve this
% setup task, which itself needs admin privileges to modify
% system files etc. "sudo" will prompt the user for his admin
% password to complete the tasks.
%
% The script also checks if any special configuration is required
% to work around Linux specific OpenGL quirks of Matlab R2014b or
% later.
%

% History:
%  6.01.2012   mk  Written.
% 16.04.2013   mk  Add info about 'dialout' group for serial port access.
% 25.01.2015   mk  Add Matlab R2014b OpenGL reconfiguration.
% 20.10.2015   mk  Make sudo handling under Octave+GUI less confusing.
% 20.10.2015   mk  Trigger auto-reload and coldplug of udev devices via udevadm.
% 25.10.2015   mk  Remember y/n answers to allow quicker reexec of the command if
%                  reexecution is needed from octave-cli or as admin user.
%                  Add option to automatically add user to dialout and psychtoolbox
%                  Unix user groups, and to add other optional users as well.
%                  Allow upgrade of the 99-psychtoolbox.limits conf file as well.
% 04.11.2015   mk  Support creation of /etc/X11/xorg.conf.d/ directory owned by
%                  group 'psychtoolbox' with setgid flag set, and write permissions,
%                  to allow members of the 'psychtoolbox' user group to add/remove/
%                  modify xorg.conf files without need for admin permissions.
% 27.06.2016   mk  Optionally install a /etc/modprobe.d/blacklist-psychtoolbox.conf
%                  file to prevent loading of certain kernel modules, e.g., the 'lp'
%                  module for control of parallel port line printers, which would prevent
%                  exclusive access to parallel ports for digital i/o.
%                  Also add users to the 'lp' group for parallel port access, and
%                  on ARM architecture (RaspberryPi) to the 'gpio' group for GPIO
%                  pin access.
% 04.09.2016   mk  Optionally install a /etc/modprobe.d/amddeepcolor-psychtoolbox.conf
%                  file to switch radeon-kms / amdgpu-kms into deep color mode for
%                  driving suitable HDMI/DP > 8 bpc displays.
% 23.06.2019   mk  Optionally install a /etc/gamemode.ini for an optionally installed
%                  FeralInteractive gamemode package, to switch supported gpu's into
%                  high-performance mode for Priority() > 0, and do possible other PTB
%                  specific optimizations beyond the default optims of the gamemode package.
rerun = 0;
updateinitramfs = 0;

if ~IsLinux
  return;
end

if nargin < 1 || isempty(answers)
  answers = '???????';
else
  if ~ischar(answers) || length(answers) ~= 7
    error('Provided input argument ''answers'' must be a 7 character string with characters y, n or ?');
  end
end

if nargin < 2 || isempty(dontshowad)
    dontshowad = 0;
end

% Retrieve login username of current user:
[ignore, username] = system('whoami'); %#ok<ASGLU>
username = username(1:end-1);
addgroup = 0;

% Setup of /etc/udev/rules.d/psychtoolbox.rules file, if needed:

fprintf('\n\nLinux specific system setup for running Psychtoolbox as non-root user:\n');
fprintf('You need to be a user with administrative rights for this function to succeed.\n');
fprintf('If you don''t have administrator rights, or if you don''t trust this script to\n');
fprintf('tinker around with system settings, simply answer all questions with "n" for "No"\n');
fprintf('and then call a system administrator for help.\n\n');
fprintf('Checking if the Psychtoolbox udev rules file is installed and up to date.\n');
fprintf('This file will allow Psychtoolbox to access special research hardware equipment,\n');
fprintf('e.g., the Cambridge Research Systems Bits+ box, the Datapixx from VPixx, response\n');
fprintf('button boxes, and some special features of your graphics card, e.g., high precision\n');
fprintf('timestamping. You will be able to access this hardware without the need to run\n');
fprintf('Matlab or Octave as sudo root user.\n\n');

% Assume no need to install or update:
answer = '';
needinstall = 0;

% Check if udev psychtoolbox.rules file exists:
if ~exist('/etc/udev/rules.d/psychtoolbox.rules', 'file')
  % No: Needs to be installed.
  needinstall = 1;
  fprintf('The udev rules file for Psychtoolbox is not installed on your system.\n');
  if ismember(answers(1), 'yn')
    answer = answers(1);
  else
    answer = input('Should i install it? [y/n] : ', 's');
    answers(1) = answer;
  end
else
  % Yes.
  fprintf('The udev rules file for Psychtoolbox is already installed on your system.\n');

  % Compare its modification date with the one in PTB:
  r = dir([PsychtoolboxRoot '/PsychBasic/psychtoolbox.rules']);
  i = dir('/etc/udev/rules.d/psychtoolbox.rules');

  if r.datenum > i.datenum
    needinstall = 2;
    fprintf('However, it seems to be outdated. I have a more recent version with me.\n');
    if ismember(answers(1), 'yn')
      answer = answers(1);
    else
      answer = input('Should i update it? [y/n] : ', 's');
      answers(1) = answer;
    end
  end
end

if needinstall && answer == 'y'
  if IsOctave && IsGUI
    rerun = 1;
    fprintf('Oops, we are running under Octave in GUI mode. This will not work!\n');
    fprintf('But not to worry. Just run Octave from a terminal window later once without\n');
    fprintf('GUI, e.g., via executing: octave --no-gui\n');
    fprintf('Then run the PsychLinuxConfiguration command again to make things work.\n');
  else
    fprintf('I will copy my most recent rules file to your system. Please enter\n');
    fprintf('now your system administrator password. You will not see any feedback.\n');
    drawnow;
    cmd = sprintf('sudo cp %s/PsychBasic/psychtoolbox.rules /etc/udev/rules.d/', PsychtoolboxRoot);
    [rc, msg] = system(cmd);
    if rc == 0
      fprintf('Success! Trying to apply the new rules file.\n');
    else
      fprintf('Failed! The error message was: %s\n', msg);
    end

    if rc == 0
        % Call udevadm to try to trigger a reload of the new rules file and trigger
        % a redetection of devices to apply the new rules without need for a reboot:
        cmd = sprintf('sudo udevadm control --reload ; sudo udevadm trigger');
        [rc, msg] = system(cmd);
        if rc == 0
          fprintf('Success! You may need to reboot your machine for some changes to take effect.\n');
        else
          fprintf('Failed! The error message was: %s\n', msg);
        end
    end
  end
end

% Check if psychtoolbox modules blacklist file exists:
fprintf('\n\n');
answer = '';
needinstall = 0;
if ~exist('/etc/modprobe.d/blacklist-psychtoolbox.conf', 'file')
  % No: Needs to be installed.
  needinstall = 1;
  fprintf('The blacklist file for Psychtoolbox is not installed on your system.\n');
  if ismember(answers(5), 'yn')
    answer = answers(5);
  else
    answer = input('Should i install it? [y/n] : ', 's');
    answers(5) = answer;
  end
else
  % Yes.
  fprintf('The blacklist file for Psychtoolbox is already installed on your system.\n');

  % Compare its modification date with the one in PTB:
  r = dir([PsychtoolboxRoot '/PsychHardware/blacklist-psychtoolbox.conf']);
  i = dir('/etc/modprobe.d/blacklist-psychtoolbox.conf');

  if r.datenum > i.datenum
    needinstall = 2;
    fprintf('However, it seems to be outdated. I have a more recent version with me.\n');
    if ismember(answers(5), 'yn')
      answer = answers(5);
    else
      answer = input('Should i update it? [y/n] : ', 's');
      answers(5) = answer;
    end
  end
end

if needinstall && answer == 'y'
  if IsOctave && IsGUI
    rerun = 1;
    fprintf('Oops, we are running under Octave in GUI mode. This will not work!\n');
    fprintf('But not to worry. Just run Octave from a terminal window later once without\n');
    fprintf('GUI, e.g., via executing: octave --no-gui\n');
    fprintf('Then run the PsychLinuxConfiguration command again to make things work.\n');
  else
    fprintf('I will copy my most recent blacklist file to your system. Please enter\n');
    fprintf('now your system administrator password. You will not see any feedback.\n');
    drawnow;
    cmd = sprintf('sudo cp %s/PsychHardware/blacklist-psychtoolbox.conf /etc/modprobe.d/', PsychtoolboxRoot);
    [rc, msg] = system(cmd);
    if rc == 0
      updateinitramfs = 1;
      fprintf('Success! You may need to reboot your machine for some changes to take effect.\n');
    else
      fprintf('Failed! The error message was: %s\n', msg);
    end
  end
end

% Check if psychtoolbox AMD kms modules config file and Mesa deep 10 bpc config exists:
fprintf('\n\n');
answer = '';
needinstall = 0;
if ~exist('/etc/modprobe.d/amddeepcolor-psychtoolbox.conf', 'file') || ...
   ~exist('/usr/share/drirc.d/90-psychtoolbox-deepcolor.conf', 'file')
  % No: Needs to be installed.
  needinstall = 1;

  if ~exist('/etc/modprobe.d/amddeepcolor-psychtoolbox.conf', 'file')
    needinstall = 3;
  end

  fprintf('The deep color config files for Psychtoolbox are not installed on your system.\n');
  fprintf('These files need to be installed if you want to use high color precision mode\n');
  fprintf('to drive a HDMI or DisplayPort high precision display device with more than\n');
  fprintf('8 bpc color depths, ie. more than 256 levels of red, green and blue color.\n');
  if IsARM
    % Some special treatment crammed in here for the latest RPi OS 11 BS:
    fprintf('\nYou also need this file on the RaspberryPi computer to get non-broken visual\n');
    fprintf('stimulation timing with RaspberryPi OS 11, so i will install the file for you.\n');
    answers(6) = 'y';
  else
    fprintf('You do not need to install it for pure 8 bpc mode.\n');
  end

  if ismember(answers(6), 'yn')
    answer = answers(6);
  else
    answer = input('Should i install them? [y/n] : ', 's');
    answers(6) = answer;
  end
else
  % Yes.
  fprintf('The deep color config files for Psychtoolbox are already installed on your system.\n');

  % Compare its modification date with the one in PTB:
  r = dir([PsychtoolboxRoot '/PsychHardware/amddeepcolor-psychtoolbox.conf']);
  i = dir('/etc/modprobe.d/amddeepcolor-psychtoolbox.conf');
  r2 = dir([PsychtoolboxRoot '/PsychHardware/90-psychtoolbox-deepcolor.conf']);
  i2 = dir('/usr/share/drirc.d/90-psychtoolbox-deepcolor.conf');

  if (r.datenum > i.datenum) || (r2.datenum > i2.datenum)
    needinstall = 2;
    if (r.datenum > i.datenum)
      needinstall = 3;
    end

    if IsARM
      % Force it for the RaspberryPi:
      answers(6) = 'y';
    end

    fprintf('However, they seem to be outdated. I have more recent versions with me.\n');
    if ismember(answers(6), 'yn')
      answer = answers(6);
    else
      answer = input('Should i update them? [y/n] : ', 's');
      answers(6) = answer;
    end
  end
end

if needinstall && answer == 'y'
  if IsOctave && IsGUI
    rerun = 1;
    fprintf('Oops, we are running under Octave in GUI mode. This will not work!\n');
    fprintf('But not to worry. Just run Octave from a terminal window later once without\n');
    fprintf('GUI, e.g., via executing: octave --no-gui\n');
    fprintf('Then run the PsychLinuxConfiguration command again to make things work.\n');
  else
    fprintf('I will copy my most recent deep color config files to your system. Please enter\n');
    fprintf('now your system administrator password. You will not see any feedback.\n');
    drawnow;
    if needinstall == 3
      cmd = sprintf('sudo cp %s/PsychHardware/amddeepcolor-psychtoolbox.conf /etc/modprobe.d/', PsychtoolboxRoot);
      [rc, msg] = system(cmd);
      if rc == 0
        updateinitramfs = 1;
        fprintf('Success! You will need to reboot your machine for this change to take effect.\n');
      else
        fprintf('Failed! The error message was: %s\n', msg);
      end
    end

    cmd = sprintf('sudo cp %s/PsychHardware/90-psychtoolbox-deepcolor.conf /usr/share/drirc.d/', PsychtoolboxRoot);
    [rc, msg] = system(cmd);
    if rc == 0
      fprintf('Success! You will need to logout and login again for this change to take effect.\n');
    else
      fprintf('Failed! The error message was: %s\n', msg);
    end
  end
end

% First the fallback implementation if /etc/security/limits.d/ does not
% exist:
if ~exist('/etc/security/limits.d/', 'dir')
% Check if /etc/security/limits.conf has proper entries to allow memory locking
% and real-time scheduling:
fid = fopen('/etc/security/limits.conf');
if fid == -1
  fprintf('Could not open /etc/security/limits.conf for reading. Can not set it up, sorry!\n');
  return;
end

fprintf('\nChecking if /etc/security/limits.conf has entries which allow everyone to\n');
fprintf('make use of realtime scheduling and memory locking -- Needed for good timing.\n\n');

mlockok = 0;
rtpriook = 0;

while ~feof(fid)
  fl = fgetl(fid);
  % fprintf('%s\n', fl);

  if fl == -1
    continue;
  end

  if ~isempty(strfind(fl, 'memlock')) && ~isempty(strfind(fl, 'unlimited')) && ~isempty(strfind(fl, '@psychtoolbox')) && ~isempty(strfind(fl, '-'))
    mlockok = 1;
  end

  if ~isempty(strfind(fl, 'rtprio')) && ~isempty(strfind(fl, '50')) && ~isempty(strfind(fl, '@psychtoolbox')) && ~isempty(strfind(fl, '-'))
    rtpriook = 1;
  end
end

% Done reading the file:
fclose(fid);

drawnow;

% ...and?
if ~(mlockok && rtpriook)
  fprintf('\n\nThe file seems to be missing some suitable setup lines.\n');
  if ismember(answers(2), 'yn')
    answer = answers(2);
  else
    answer = input('Should i add them for you? [y/n] : ', 's');
    answers(2) = answer;
  end

  if answer == 'y'
    if IsOctave && IsGUI
      rerun = 1;
      fprintf('Oops, we are running under Octave in GUI mode. This will not work!\n');
      fprintf('But not to worry. Just run Octave from a terminal window later once without\n');
      fprintf('GUI, e.g., via executing: octave --no-gui\n');
      fprintf('Then run the PsychLinuxConfiguration command again to make things work.\n');
    else
      fprintf('I will try to add config lines to your system. Please enter\n');
      fprintf('now your system administrator password. You will not see any feedback.\n');
      drawnow;

      % Set amount of lockable memory to unlimited for all users:
      [rc, msg] = system('sudo /bin/bash -c ''echo "@psychtoolbox     -     memlock     unlimited" >> /etc/security/limits.conf''');
      if rc ~= 0
        fprintf('Failed! The error message was: %s\n', msg);
      end

      % Set allowable realtime priority for all users to 50:
      [rc2, msg] = system('sudo /bin/bash -c ''echo "@psychtoolbox     -     rtprio      50" >> /etc/security/limits.conf''');
      if rc2 ~= 0
        fprintf('Failed! The error message was: %s\n', msg);
      end

      % Must add a psychtoolbox user group:
      addgroup = 1;

      if (rc == 0) && (rc2 == 0)
        fprintf('\n\nSuccess!\n\n');
      else
        fprintf('\n\nFailed! Maybe ask a system administrator for help?\n\n');
      end
    end
  end
else
  fprintf('\n\nYour system is already setup for use of Priority().\n\n');
end

else
  % Realtime setup for systems with /etc/security/limits.d/ directory:
  % Simply install a ptb specific config file - a cleaner solution:
  donttouchthis = 1;
  if ~exist('/etc/security/limits.d/99-psychtoolboxlimits.conf', 'file')
    donttouchthis = 0;
  else
    r = dir([PsychtoolboxRoot '/PsychBasic/99-psychtoolboxlimits.conf']);
    i = dir('/etc/security/limits.d/99-psychtoolboxlimits.conf');

    if r.datenum > i.datenum
      donttouchthis = 0;
    end
  end

  if ~donttouchthis
    fprintf('\n\nThe file /etc/security/limits.d/99-psychtoolboxlimits.conf is\n');
    fprintf('not yet installed or outdated on your system. It allows painless realtime operation.\n');

    if ismember(answers(2), 'yn')
      answer = answers(2);
    else
      answer = input('Should i install or upgrade to my most recent file for you? [y/n] : ', 's');
      answers(2) = answer;
    end

    if answer == 'y'
      if IsOctave && IsGUI
        rerun = 1;
        fprintf('Oops, we are running under Octave in GUI mode. This will not work!\n');
        fprintf('But not to worry. Just run Octave from a terminal window later once without\n');
        fprintf('GUI, e.g., via executing: octave --no-gui\n');
        fprintf('Then run the PsychLinuxConfiguration command again to make things work.\n');
      else
        fprintf('I will try to install it now to your system. Please enter\n');
        fprintf('now your system administrator password. You will not see any feedback.\n');
        drawnow;
        cmd = sprintf('sudo cp %s/PsychBasic/99-psychtoolboxlimits.conf /etc/security/limits.d/', PsychtoolboxRoot);
        [rc, msg] = system(cmd);
        if rc ~= 0
          fprintf('Failed! The error message was: %s\n', msg);
        else
          fprintf('Success!\n\n');

          % Must add a psychtoolbox user group:
          addgroup = 1;
        end
      end
    end
  end
end

% Need to create a Unix user group 'psychtoolbox' and add user to it?
if addgroup
  fprintf('I will try to create a new Unix user group called "psychtoolbox" on your system.\n');
  if IsOctave && IsGUI
    rerun = 1;
    fprintf('Oops, we are running under Octave in GUI mode. This will not work!\n');
    fprintf('But not to worry. Just run Octave from a terminal window later once without\n');
    fprintf('GUI, e.g., via executing: octave --no-gui\n');
    fprintf('Then run the PsychLinuxConfiguration command again to make things work.\n');
    addgroup = 0;
  else
    % This will create the psychtoolbox user group, unless the group
    % already exists. In such a case it simply does nothing:
    system('sudo groupadd --force psychtoolbox');

    fprintf('I have created a new Unix user group called "psychtoolbox" on your system.\n');
    addgroup = 1;
  end
else
  fprintf('\n\nYour system has a Unix user group called "psychtoolbox".\n');
  addgroup = 1;
end

if addgroup
  fprintf('All members of that group can use the Priority() command now without the need\n');
  fprintf('to run Matlab or Octave as sudo root user.\n\n');
  fprintf('You need to add each user of Psychtoolbox to that group. You could do this\n');
  fprintf('with the user management tools of your system. Or you can open a terminal window\n');
  fprintf('and type the following command (here as an example to add yourself to that group):\n\n');
  fprintf('sudo usermod -a -G psychtoolbox %s\n\n', username);
  fprintf('One should also add oneself to the ''dialout'' group for access to serial port devices:\n\n');
  fprintf('sudo usermod -a -G dialout %s\n\n', username);
  fprintf('One should also add oneself to the ''lp'' group for access to parallel port devices:\n\n');
  fprintf('sudo usermod -a -G lp %s\n\n', username);
  fprintf('One should also add oneself to the ''video'' group for use on hybrid graphics laptops:\n\n');
  fprintf('sudo usermod -a -G video %s\n\n', username);
  if IsARM
    fprintf('One should also add oneself to the ''gpio'' group for access to GPIO pins:\n\n');
    fprintf('sudo usermod -a -G gpio %s\n\n', username);
  end
  fprintf('After that, the new group member must log out and then login again for the\n');
  fprintf('settings to take effect.\n\n');
  if ismember(answers(3), 'yn')
    answer = answers(3);
  else
    fprintf('Actually we could do this for you right now.\n');
    answer = input('Should i add you to the user groups proposed above? [y/n] : ', 's');
    answers(3) = answer;
  end

  if answer == 'y'
    againagain = 1;
    while againagain
      fprintf('\nAdding user %s to groups...\n', username);
      cmd = sprintf('sudo usermod -a -G psychtoolbox %s', username);
      system(cmd);
      cmd = sprintf('sudo usermod -a -G dialout %s', username);
      system(cmd);
      cmd = sprintf('sudo usermod -a -G lp %s', username);
      system(cmd);
      cmd = sprintf('sudo usermod -a -G video %s', username);
      system(cmd);

      % On RaspberryPi also add to the gpio group for GPIO access:
      if IsARM
        cmd = sprintf('sudo usermod -a -G gpio %s', username);
        system(cmd);
      end

      % Another one?
      username = input('Enter the name of another user to add, or just press Return to be done: ', 's');
      if isempty(username)
        againagain = 0;
      end
    end
    fprintf('\n');
  end
end

if addgroup
  % Check if xorg.conf.d exists, but does not have psychtoolbox as owning group.
  % In that case we need to chown + chmod it to ownership and write permission for
  % psychtoolbox group. This happens, e.g., on RaspberryPi OS 12, when the system
  % by itself creates the directory to place some config files, but not with psychtoolbox
  % ownership. Need different ls() output string handling for Octave vs. Matlab for this:
  if IsOctave
    xorgconfdir = vec(ls('/etc/X11/', '-l')')';
  else
    xorgconfdir = ls('/etc/X11/', '-l');
  end
  xorgnotptbowned = exist('/etc/X11/xorg.conf.d', 'dir') && isempty(strfind(xorgconfdir, 'psychtoolbox'));

  % Does not exist or is not group owned by us?
  if ~exist('/etc/X11/xorg.conf.d', 'dir') || xorgnotptbowned
    if xorgnotptbowned
      fprintf('\n\nThe X11 configuration directory /etc/X11/xorg.conf.d is not owned by\n');
      fprintf('the psychtoolbox Unix group. I would like to change that, to allow you\n');
    else
      fprintf('\n\nThe X11 configuration directory /etc/X11/xorg.conf.d does not exist.\n');
      fprintf('I would like to create it and allow users of the psychtoolbox Unix group\n');
    end
    fprintf('to write xorg config files into that directory. This will allow easy change\n');
    fprintf('of display configuration when needed to setup for an experiment by use of the\n');
    fprintf('XOrgConfCreator and XOrgConfSelector functions of Psychtoolbox.\n\n');

    if ismember(answers(4), 'yn')
      answer = answers(4);
    else
      answer = input('Should i create or configure a X11 config directory that is writable for you? [y/n] : ', 's');
      answers(4) = answer;
    end

    if answer == 'y'
      if IsOctave && IsGUI
        rerun = 1;
        fprintf('Oops, we are running under Octave in GUI mode. This will not work!\n');
        fprintf('But not to worry. Just run Octave from a terminal window later once without\n');
        fprintf('GUI, e.g., via executing: octave --no-gui\n');
        fprintf('Then run the PsychLinuxConfiguration command again to make things work.\n');
      else
        if ~exist('/etc/X11/xorg.conf.d', 'dir')
          system('sudo mkdir /etc/X11/xorg.conf.d');
        end
        system('sudo chown :psychtoolbox /etc/X11/xorg.conf.d');
        system('sudo chmod g+rwxs /etc/X11/xorg.conf.d');
      end
    end
  end
end

% Check if psychtoolbox gamemode.ini file exists:
fprintf('\n\n');
answer = '';
needinstall = 0;
if ~exist('/etc/gamemode.ini', 'file') && ~exist('/usr/share/gamemode/gamemode.ini', 'file') && ...
   (isempty(getenv('XDG_CONFIG_HOME')) || ~exist([getenv('XDG_CONFIG_HOME') '/gamemode.ini'], 'file')) && ...
   ~exist([getenv('HOME') '/.config/gamemode.ini'], 'file')
  % No: Needs to be installed.
  needinstall = 1;
  fprintf('The gamemode.ini file customized for Psychtoolbox is not installed on your system.\n');
  fprintf('You need this file if you install(ed) the optional \"FeralInteractive gamemode\"\n');
  fprintf('package to further optimize your system for realtime and high performance during\n');
  fprintf('Priority(>0) operation *and* you want the gamemode package to also switch supported\n');
  fprintf('graphics cards into high performance mode for an extra boost in graphics performance,\n');
  fprintf('at the cost of higher power consumption and heat production. PLEASE NOTE that in case\n');
  fprintf('of faulty or sub-standard cooling of your graphics card or computer, there is a SMALL\n');
  fprintf('RISK OF HARDWARE DAMAGE DUE TO OVERHEATING if you say yes to this question.\n\n');
  fprintf('ANY DAMAGE TO HARDWARE INCURRED DUE TO THIS FEATURE IS YOUR RESPONSIBILITY AND YOURS ALONE!\n\n');
  fprintf('Say no to the question if you don''t want extra graphics performance optimizations,\n');
  fprintf('don''t want to take the SMALL RISK OF OVERHEATING, or do already have your own customized\n');
  fprintf('gamemode.ini file installed in a location other than /etc/gamemode.ini.\n\n')

  if ismember(answers(7), 'yn')
    answer = answers(7);
  else
    answer = input('Should i install it? [y/n] : ', 's');
    answers(7) = answer;
  end
else
  % Yes. File already exists in one location. Is it our location?
  if exist('/etc/gamemode.ini', 'file')
    fprintf('The gamemode.ini file is already installed on your system at the location expected by Psychtoolbox.\n');

    % Compare its modification date with the one in PTB:
    r = dir([PsychtoolboxRoot '/PsychBasic/gamemode.ini']);
    i = dir('/etc/gamemode.ini');

    if r.datenum > i.datenum
      needinstall = 2;
      fprintf('However, it seems to be outdated. I have a more recent version with me. This is the difference\n');
      fprintf('between the installed version and my version:\n');
      system(['diff /etc/gamemode.ini ' PsychtoolboxRoot '/PsychBasic/gamemode.ini']);
      fprintf('\n');
      if ismember(answers(7), 'yn')
        answer = answers(7);
      else
        answer = input('Should i update/replace it with my version? [y/n] : ', 's');
        answers(7) = answer;
      end
    end
  else
    % Some 3rd party file exists in different location:
    fprintf('A 3rd party gamemode.ini file is already installed on your system. Not touching it.\n');
  end
end

if needinstall && answer == 'y'
  if IsOctave && IsGUI
    rerun = 1;
    fprintf('Oops, we are running under Octave in GUI mode. This will not work!\n');
    fprintf('But not to worry. Just run Octave from a terminal window later once without\n');
    fprintf('GUI, e.g., via executing: octave --no-gui\n');
    fprintf('Then run the PsychLinuxConfiguration command again to make things work.\n');
  else
    % Actually check if the gamemode package is installed on the system, by
    % checking if gamemoded is there:
    r = system('which gamemoded');
    if r ~= 0
      % Seems to be missing. Propose installing it:
      fprintf('I could not find the gamemoded executable in the path. This probably means\n');
      fprintf('that the gamemode package is not installed yet?\n');
      fprintf('If this is an Ubuntu system of version 18.04-LTS or later, you can try\n');
      fprintf('to install the package from a *untrusted* 3rd party PPA repository\n');
      fprintf('for your system. See \"help LinuxGameMode\" on how to do that.\n');
      fprintf('Skipping the setup of gamemode.ini for now. Rerun PsychLinuxConfiguration\n');
      fprintf('after you have installed the gamemode package.\n');
      fprintf('Press a key to confirm you read above message.\n');
      pause;
    else
      % gamemoded installed. Is the gpuclockctl executable installed as well?
      fprintf('I will now copy my most recent gamemode.ini file to your system. Please enter\n');
      fprintf('your system administrator password. You will not see any feedback.\n');
      drawnow;
      cmd = sprintf('sudo cp %s/PsychBasic/gamemode.ini /etc/gamemode.ini', PsychtoolboxRoot);
      [rc, msg] = system(cmd);
      if rc == 0
        % Sucess: kill a potentially running gamemoded, so it will get restarted as needed and
        % reread our updated or new gamemode.ini config file:
        system('killall gamemoded');
        fprintf('Success! Changes should apply at next use of the Priority() command, but the latest after a logout/login or reboot.\n');
      else
        fprintf('Failed! The error message was: %s\n', msg);
      end

      if ~exist('/usr/lib/x86_64-linux-gnu/gpuclockctl', 'file') && exist('/usr/lib/x86_64-linux-gnu/cpugovctl', 'file')
        % gpuclockctl seems to be missing in the expected location while its sibling cpugovctl exists?
        % This suggests dealing with a buggy gamemode package that is missing gpuclockctl for some weird reason,
        % e.g., the v1.3.1 package from https://launchpad.net/~samoilov-lex/+archive/ubuntu/gamemode.
        %
        % Propose to the user to fix the missing file from our own copy (built on 19.04, but works on 18.04 LTS):
        fprintf('The gamemode package seems to be installed, but missing the following required file:\n');
        fprintf('/usr/lib/x86_64-linux-gnu/gpuclockctl\n\n');
        fprintf('I have a copy of this file with me that works on at least Ubuntu 19.04 and Ubuntu 18.04 LTS\n');
        fprintf('with gamemode PPA version 1.3.1.\n');
        answer = input('Should i update/replace the missing file with my version? [y/n] : ', 's');
        if answer == 'y'
          cmd = sprintf('sudo cp %s/PsychContributed/gpuclockctl /usr/lib/x86_64-linux-gnu/gpuclockctl', PsychtoolboxRoot);
          [rc, msg] = system(cmd);
          if rc == 0
            fprintf('Success! GPU performance optimizations at Priority() > 0 should now hopefully work.\n');
          else
            fprintf('Failed! Giving up. You may need to fix this yourself. The error message was:\n%s\n', msg);
            fprintf('If the file is missing, e.g., from a NeuroDebian/Debian/Ubuntu package, you can\n');
            fprintf('get a copy from the following download link, to put it into the PsychContributed\n');
            fprintf('subfolder, then rerun PsychLinuxConfiguration:\n');
            fprintf('https://github.com/Psychtoolbox-3/Psychtoolbox-3/raw/master/Psychtoolbox/PsychContributed/gpuclockctl\n\n');
          end
        end
      end
    end
  end
end

if updateinitramfs
  fprintf('\nNow updating the initramfs for some settings to take effect. This can take some time.\n');
  system('sudo update-initramfs -u -k all');
end

if ~rerun
  fprintf('\nFinished. Your system should now be ready for use with Psychtoolbox.\n');
  fprintf('If you encounter problems, try rebooting the machine. Some of the settings only\n');
  fprintf('become effective after a reboot.\n\n\n');
else
  fprintf('\nSetup failed due to lack of administrator permissions. Please run octave\n');
  fprintf('from a terminal window via ''octave --no-gui'' while logged in as a user with\n');
  fprintf('administrator capabilities.\n');
  fprintf('Then execute the command\n\nPsychLinuxConfiguration(''%s'');\n\n', answers);
  fprintf('to rerun this script again with the same settings, so you can avoid\n');
  fprintf('answering all yes/no questions again. Then it should work.\n\n\n');
end

fprintf('Press Return key to continue.\n');
pause;
fprintf('\n\n\n');

% Matlab version 8.4 (R2014b) or later?
if ~IsOctave && exist('verLessThan') && ~verLessThan('matlab', '8.4.0') %#ok<EXIST>
  % Yes.

  % Latest Matlab bundles a totally crippled libvulkan.so.1 Vulkan loader
  % that overrides the good system installed Vulkan loader and sabotages
  % our PsychVulkan support. Apparently competent people are hard to hire...
  % Try to detect this and and rename the file:
  libvulkan = ([matlabroot '/bin/glnxa64/libvulkan.so.1']);
  if exist(libvulkan, 'file')
    cmd = ['sudo mv ' libvulkan ' ' libvulkan '.DISABLED'];
    fprintf('\nThis Matlab version ships a broken libvulkan.so.1 library. Renaming it to disable it...\n');
    fprintf('You may have to enter your administrator password to execute the following command:\n%s\n\n', cmd);
    system(cmd);

    if exist(libvulkan, 'file')
      warning('Failed to rename Matlabs broken libvulkan.so.1. PsychVulkan will not work!');
    else
      fprintf('Success. Vulkan should work now.\n');
    end
  end

  % If R2014b detects a Mesa OpenGL renderer as default system OpenGL
  % library, it will blacklist it and switch to its own utterly outdated
  % Mesa X11 software renderer (Version 7.2 !!). This is utterly inadequate
  % for our purpose, therefore lets reconfigure Matlab to force it to use the
  % system provided hardware accelerated Mesa OpenGL library after the next
  % Matlab restart.
  d = opengl('data');
  if (ischar(d.Software) && ~isempty(strfind(d.Software, 'true'))) || (isscalar(d.Software) && d.Software)
    % Software renderer. Let's change this for future Matlab sessions:
    fprintf('Your version of Matlab R2014b or later was setup to use its own software OpenGL renderer.\n');
    try
        opengl('save','hardware');
        fprintf('As this is completely unsuitable for Psychtoolbox, i have changed the setting to use the\n');
        fprintf('system provided hardware accelerated OpenGL library. You need to quit and restart Matlab\n');
        fprintf('for these changes to take effect, otherwise visual stimulation will be broken.\n\n');
    catch
        fprintf('This is completely unsuitable for Psychtoolbox! You may need to quit and restart Matlab\n');
        fprintf('with the command line switch ''matlab -nosoftwareopengl'' to fix this each time.\n');
    end
    fprintf('Press any key to continue to confirm you read and understood this.\n');
    pause;
    fprintf('\n\n');
  end
end

% Return the given answers / used config:
usedAnswers = answers;

% Trigger libdc1394 workarounds if Screen does not work. Postinstall routine would
% run this, but a NeuroDebian install would not, so call it here to handle ND as well:
AssertOpenGL;

% Our little ad for our services:
if ~dontshowad && exist('PsychPaidSupportAndServices', 'file')
    PsychPaidSupportAndServices(1);
end

return;
