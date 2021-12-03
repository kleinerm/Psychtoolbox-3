#!/bin/bash
#
# Go through the basic moves of updating PTB online docs
# from git tree.
#
cd ~/projects/OpenGLPsychtoolbox/psychtoolbox-3.github.com/
git pull
cd ~/projects/OpenGLPsychtoolbox/Psychtoolbox-3/Psychtoolbox
../managementtools/PTB-wikify-into-files.py -r -m -o ~/projects/OpenGLPsychtoolbox/psychtoolbox-3.github.com/docs/ ./
cd ~/projects/OpenGLPsychtoolbox/psychtoolbox-3.github.com/
git status
