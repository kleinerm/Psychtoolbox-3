function makeftglplugin
% Build the FTGL text renderer plugin and put it into the
% PsychPlugins folder.

if IsWin
    % 64-Bit build with MSVC 2019, done from a separate MS-DOS batch script:
    cmd = 'buildftglpluginformsvc64Bit.bat';
    name = 'libptbdrawtext_ftgl64.dll';

    % Does our own self-buit fontconfig.lib import file already exist?
    % UPDATE: This was needed for building against GStreamer 1.20, but
    % is not needed right now for GStreamer 1.22, so disable:
    if ~exist('fontconfig.lib', 'file') && false
        % No. Need to build it ourselves, as the one shipping with
        % GStreamer 1.20.5 is broken!
        fprintf('Generating our own fontconfig-1.lib import library now...\n');

        % 1. Extract fontconfig function definitions from GStreamer's
        % fontconfig-1.dll into a file. Our build batch file does this
        % as a side-effect:
        [rc, ret] = system(cmd);
        if rc ~= 0 || ~exist('fontconfig-1.functions', 'file')
            disp(ret);
            error('Failed to parse out fontconfig function definitions!');
        end

        % Parse the file and turn it into a linker .def file:
        f = fopen('fontconfig-1.functions');
        fo=fopen('fontconfig-1.def', 'wt');
        fprintf(fo, 'EXPORTS\n');
        while ~feof(f)
            l = fgetl(f);
            lo = l(strfind(l, 'Fc'):end);
            if ~isempty(lo)
                fprintf(fo, '%s\n', lo);
            end
        end
        fclose(f);
        fclose(fo);

        if ~exist('fontconfig-1.def', 'file')
            disp(ret);
            error('Failed to generate fontconfig-1.def file!');
        end

        % Ok, got the file. A successive 2nd invocation of our batch
        % file will turn that fontconfig-1.def file into a
        % fontconfig-1.lib library import definition file for use by
        % the linker while building our FTGL plugin, to finally use the
        % right fontconfig-1.dll library from GStreamer in the end.
    end
end

if IsOSX
    cmd = 'g++ -g -DHAVE_OPENGL_DIR -I.  -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/ -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/freetype2/ -L/Library/Frameworks/GStreamer.framework/Versions/Current/lib/ -framework OpenGL -l fontconfig -l freetype -dynamiclib -o libptbdrawtext_ftgl64.dylib libptbdrawtext_ftgl.cpp qstringqcharemulation.cpp OGLFT.cpp';
    name = 'libptbdrawtext_ftgl64.dylib';
end

if IsLinux
    if Is64Bit
        cmd='g++ -g -fPIC -I. -I/usr/include/ -I/usr/include/freetype2/ -L/usr/lib -pie -shared -Wl,-Bsymbolic -Wl,-Bsymbolic-functions -Wl,--version-script=linuxexportlist.txt -o libptbdrawtext_ftgl64.so.1 libptbdrawtext_ftgl.cpp qstringqcharemulation.cpp OGLFT.cpp -lGL -lGLU -lfontconfig -lfreetype';
        name = 'libptbdrawtext_ftgl64.so.1';
    else
        if IsARM
            cmd='g++ -g -fPIC -I. -I/usr/include/ -I/usr/include/freetype2/ -L/usr/lib -pie -shared -Wl,-Bsymbolic -Wl,-Bsymbolic-functions -Wl,--version-script=linuxexportlist.txt -o libptbdrawtext_ftgl_arm.so.1 libptbdrawtext_ftgl.cpp qstringqcharemulation.cpp OGLFT.cpp -lGL -lGLU -lfontconfig -lfreetype';
            name = 'libptbdrawtext_ftgl_arm.so.1';
        else
            cmd='g++ -g -fPIC -I. -I/usr/include/ -I/usr/include/freetype2/ -L/usr/lib -pie -shared -Wl,-Bsymbolic -Wl,-Bsymbolic-functions -Wl,--version-script=linuxexportlist.txt -o libptbdrawtext_ftgl.so.1 libptbdrawtext_ftgl.cpp qstringqcharemulation.cpp OGLFT.cpp -lGL -lGLU -lfontconfig -lfreetype';
            name = 'libptbdrawtext_ftgl.so.1';
        end
    end
end

if ~isempty(cmd)
    [rc, ret] = system(cmd);
    disp(ret);
    if rc == 0
        movefile(name, [PsychtoolboxRoot 'PsychBasic' filesep 'PsychPlugins']);
        fprintf('Success.\n');
    else
        fprintf('FAILED!\n');
    end
end

if IsWin && exist('fontconfig-1.functions', 'file')
    delete('fontconfig-1.functions');
end

return;
