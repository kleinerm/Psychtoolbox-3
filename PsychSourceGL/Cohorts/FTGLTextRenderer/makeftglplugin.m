function makeftglplugin
% Build the FTGL text renderer plugin and put it into the
% PsychPlugins folder.

if IsWin
    if IsOctave
        if Is64Bit
            % 64-Bit build with gcc:
            cmd = 'g++ -g -m64 -o libptbdrawtext_ftgl64.dll -fno-exceptions -shared -LC:\gstreamer\1.0\x86_64\lib -I. -IC:\gstreamer\1.0\x86_64\include\freetype2 -IC:\gstreamer\1.0\x86_64\include\fontconfig libptbdrawtext_ftgl.cpp qstringqcharemulation.cpp OGLFT.cpp -lopengl32 -lglu32 -lfontconfig -lfreetype'
            name = 'libptbdrawtext_ftgl64.dll';
        else
            % 32-Bit build with gcc:
            cmd = 'g++ -g -o libptbdrawtext_ftgl.dll -fno-exceptions -shared -LC:\gstreamer\1.0\x86\lib -I. -IC:\gstreamer\1.0\x86\include\freetype2 -IC:\gstreamer\1.0\x86\include\fontconfig libptbdrawtext_ftgl.cpp qstringqcharemulation.cpp OGLFT.cpp -lopengl32 -lglu32 -lfontconfig -lfreetype';
            name = 'libptbdrawtext_ftgl.dll';
        end
    else
        % 64-Bit build with MSVC 2010, done from a separate MS-DOS batch script:
        cmd = 'buildftglpluginformsvc64Bit.bat';
        name = 'libptbdrawtext_ftgl64.dll';
    end
end

if IsOSX
    cmd = 'g++ -g -DHAVE_OPENGL_DIR -I.  -I/usr/X11/include/ -I/usr/X11/include/freetype2/ -L/usr/X11/lib/ -framework OpenGL -l fontconfig -l freetype -dynamiclib -o libptbdrawtext_ftgl64.dylib libptbdrawtext_ftgl.cpp qstringqcharemulation.cpp OGLFT.cpp';
    %cmd = 'g++ -g -DHAVE_OPENGL_DIR -I.  -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/ -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/freetype2/ -L/Library/Frameworks/GStreamer.framework/Versions/Current/lib/ -framework OpenGL -l fontconfig -l freetype -dynamiclib -o libptbdrawtext_ftgl64.dylib libptbdrawtext_ftgl.cpp qstringqcharemulation.cpp OGLFT.cpp';
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
    if rc == 0
        movefile(name, [PsychtoolboxRoot 'PsychBasic' filesep 'PsychPlugins']);
        disp(ret);
        fprintf('Success.\n');
    else
        disp(ret);
        fprintf('FAILED!\n');
    end
end

return;
