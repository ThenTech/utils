import subprocess
import fileinput
import re
import os

LF = "\n" if os.name == "nt" else "\r\n"
    
OFFSET = 23

MAIN         = "./main.cpp"
MAIN_MATCH   = "VERSION("

README       = "./README.md"
README_MATCH = "badge/version-"

GIT_COMMIT_CMD   = ['git', 'rev-list', '--all', '--count']
GIT_COMMIT_COUNT = str(int(subprocess.check_output(GIT_COMMIT_CMD)) + 1 - OFFSET)

try:
    for line in fileinput.FileInput(MAIN, inplace=1):
        if MAIN_MATCH in line:
            nline = line.split(',')
            nline[1] = " " + GIT_COMMIT_COUNT
            nline = ','.join(nline)

            line = nline
        print(line.rstrip(), end=LF)
except Exception as e:
    fileinput.close()
    print(e)

try:
    for line in fileinput.FileInput(README, inplace=1):
        if README_MATCH in line:
            nline = line.split(README_MATCH)
            dots = nline[1].split('.')
            dots[1] = GIT_COMMIT_COUNT
            nline[1] = '.'.join(dots)
            nline = README_MATCH.join(nline)

            line = nline
        print(line.rstrip(), end=LF)
except Exception as e:
    fileinput.close()
    print(e)
