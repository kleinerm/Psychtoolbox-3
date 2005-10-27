#!/bin/sh
# Copy Foo.mexmac which we built into Applications/Psychtoolbox/PsychBar directory.   
cp "$TARGET_BUILD_DIR/$TARGET_NAME.mexmac.app/Contents/MacOS/$TARGET_NAME.mexmac" "$SYSTEM_APPS_DIR/Psychtoolbox/$PSYCHTOOLBOX_SUBFOLDER"

