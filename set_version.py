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
RM_MATCH_POST= "-brightgreen"

RESOURCE     = "./resources.rc"

GIT_COMMIT_CMD   = ['git', 'rev-list', '--all', '--count']
GIT_COMMIT_COUNT = str(int(subprocess.check_output(GIT_COMMIT_CMD)) + 1 - OFFSET)

# Major.Minor.Patch-extra
VERSION = (0, 0, 0, "")

def split_version(line):
    global VERSION
    pre, version  = line.split('(')
    version, post = version.split(')')
    major, minor, patch, extra = version.split(', ')

    minor = GIT_COMMIT_COUNT
    VERSION = (int(major), int(minor), int(patch), extra)

    return pre, version, post


def version_string(delim='.', edelim='-', simple=True):
    if not edelim or not VERSION[-1]:
        return "{2}{0}{3}{0}{4}{0}0".format(delim, edelim, *VERSION)
    else:
        newVersion = VERSION
        if simple:
            extra = VERSION[3].lower()
            if "alpha" in extra:
                extra = "alpha"
            elif "beta" in extra:
                extra = "beta"
            newVersion = (newVersion[0], newVersion[1], newVersion[2], extra)

        return "{2}{0}{3}{0}{4}{1}{5}".format(delim, edelim, *newVersion)

# Acquire version from main.cpp
try:
    for line in fileinput.FileInput(MAIN, inplace=1):
        if MAIN_MATCH in line:
            pre, v, post = split_version(line)
            line = pre + '(' + version_string(", ", ", ", False) + ')' + post
        print(line.rstrip(), end=LF)
except Exception as e:
    fileinput.close()
    print(e)

print("change version to: " + version_string(", ", ", ", False))
print("change version to: " + version_string(".", "-", True))

# Set version in README
try:
    for line in fileinput.FileInput(README, inplace=1):
        if README_MATCH in line:
            pre, version  = line.split(README_MATCH)
            version, post = version.split(RM_MATCH_POST)
            line = pre + README_MATCH \
                 + version_string(".", "--") \
                 + RM_MATCH_POST + post
        print(line.rstrip(), end=LF)
except Exception as e:
    fileinput.close()
    print(e)

# Set version in Resources
try:
    for line in fileinput.FileInput(RESOURCE, inplace=1):
        if "FILEVERSION" in line or "PRODUCTVERSION" in line:
            splits = line.split(' ')
            splits[-1] = version_string(",", "")
            line = ' '.join(splits)
        elif "FileVersion" in line or "ProductVersion" in line:
            pre, _ = line.split(', ')
            line = pre + ", \"" + version_string(".", "-") + "\""
        print(line.rstrip(), end=LF)
except Exception as e:
    fileinput.close()
    print(e)
