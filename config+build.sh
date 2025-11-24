#! /bin/bash
set -u
readonly PROGNAM="${BASH_SOURCE##*/}"

errmsg()
{
	echo "${PROGNAM}: $1" >&2
}

usage()
{
	echo "Usage: ${PROGNAM} <debug>|<release>"
	echo "       CMake configures and subsequently builds either a debug or a release"
	echo "       version from CMakeLists.txt and CMakePresets.json in current directory"
	echo "       in a corresponding 'debug' or 'release' subdirectory of './build'."
}

if [[ $# -lt 1 ]]
then
	usage
	exit 1
fi

readonly CMAKE=/usr/bin/cmake
if [[ ! -x ${CMAKE} ]]
then
	errmsg "[E] Required program '${CMAKE}' not found or not executable!"
	usage >&2
	exit 1
fi
fail=0
for file in ./CMakeLists.txt ./CMakePresets.json
do
	if [[ ! -r ${file} ]]
	then
		errmsg "[E] Required input file '${file}' not found or not readable!" 
		fail=1
	fi
done
if [[ ${fail} -eq 1 ]]
then
	usage >&2
	exit 1
fi
if [[ $1 != "debug" && $1 != "release" ]]
then
	errmsg "[E] Unsupported build-mode '$1' specified!"
	usage >&2
	exit 1
fi
if ${CMAKE} --preset $1
then
	${CMAKE} --build --preset $1 
fi

