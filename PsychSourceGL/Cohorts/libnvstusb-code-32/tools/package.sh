#!/bin/sh
#svn co https://libnvstusb.svn.sourceforge.net/svnroot/libnvstusb libnvstusb.pack
#pushd libnvstusb.pack
# Edit debian/changelog with dch
dch -i
#Generate source package
DIST=maverick svn-buildpackage --svn-builder="debuild" -S -sa --svn-ignore-new
#popd
#PUSH package to PPA: dput ppa:johaahn/libnvstusb ../build-area/libnvstusb_xxxxx_source.changes

# WITH GIT:
# Create packaging branch: packaging
# Add tag of version ex: 0.0.7
# Add tag of debian version ex: debian/0.0.7-1
# Update changelog : git-dch --release --debian-branch=packaging
# Generate source package: DIST=maverick git-buildpackage --git-upstream-branch=master --git-debian-branch=packaging --git-builder="debuild -i\.git -I.git" -S -sa --git-ignore-new
 # Push package to PPA: dput ppa:johaahn/libnvstusb ../build-area/libnvstusb_xxxxx_source.changes

