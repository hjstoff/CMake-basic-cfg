#! /bin/bash
#set -u
readonly PROGNAM="${BASH_SOURCE[0]##*/}"

errmsg()
{
	echo "${PROGNAM}: $1" >&2
}

#
# Define dependencies on external executables and verify their availability.
#
readonly CAT='/usr/bin/cat'
readonly CHMOD='/usr/bin/chmod'
readonly CP='/usr/bin/cp' 
readonly GAWK='/usr/bin/gawk' 
readonly GIT='/usr/bin/git'
readonly MKDIR='/usr/bin/mkdir'
readonly REALPATH='/usr/bin/realpath'
chkdeps()
{
	local fail=0
	local executable
	for executable in ${CAT} ${CHMOD} ${CP} ${GAWK} ${GIT} ${MKDIR} ${REALPATH}
	do
		if [[ ! -x ${executable} ]]
		then
			errmsg "[E] 'required executable '${executable}' not found or not executable!"
			fail=1
		fi
	done
	if [[ $fail -ne 0 ]]
	then
		exit 1
	fi
}
chkdeps

usage()
{
"${CAT}"<<EOF
Usage: ${PROGNAM} <path-to-new-projectdir>
       Create the specified new project directory, subsequently
       prepare it for a new 'CMake basic cfg' project
EOF
}

#
# Retrieve the canonical name of the directory in which the currently
# running script is located into global readonly scalar SCRIPTDIR.
#
scriptdir()
{
	#
	# N.B.: Effective error detection REQUIRES that the declaration as local
	# and the assignment of the realpath outcome remain two distinct commands.
	#
	local currentscript
	currentscript=$(${REALPATH} "${BASH_SOURCE[0]}")
	if [[ $? -ne 0 ]]
	then
		errmsg "[E] failed to determine canonical path of currently executing script!"
		exit 1
	fi
	readonly SCRIPTDIR="${currentscript%/*}"
}
scriptdir

#
# Retrieve the canonical name of the directory from which the currently
# running script has been invoked into global readonly scalar INVOCATIONDIR.
# This of course assumes that prior to calling this function no changing of
# directory # has occurred.
#
invocationdir()
{
	INVOCATIONDIR=$(${REALPATH} "${PWD}")
	if [[ $? -ne 0 ]]
	then
		errmsg "[E] failed to determine canonical path of directory from which this script is run!"
		exit 1
	fi
	readonly INVOCATIONDIR
}
invocationdir

validate_scriptdir()
{
	local fail=0
	# Contents expected / required in scriptdir, besides the current script
	local required_files="CMakeLists.txt.template CMakePresets.json.template config+build.sh Doxyfile update-cmake-basic-cfg.sh"  
	local file
	for file in ${required_files}
	do
		if [[ ! -r "${SCRIPTDIR}/${file}" ]]
		then
			errmsg "[E] required file '${file}' in directory '${SCRIPTDIR}' not found or not readable!"
			fail=1
		fi
	done
	if [[ ! -d "${SCRIPTDIR}/.git" ]]
	then
		errmsg "[E] directory '${SCRIPTDIR}' has no '.git' subdirectory! not a checked out git repository?!"  
		fail=1
	fi
	if [[ $fail -ne 0 ]]
	then
		exit 1
	fi
}
validate_scriptdir

#
# Retrieve git remote url of the repository containing the currently running script
# into global readonly scalar SUBMODULE_URL.
#
submodule_url()
{
	local fail=0
	if ! cd "${SCRIPTDIR}" 
	then
		errmsg "[E] failed to make '${SCRIPTDIR}' the current directory!"
		exit 1
	fi
	SUBMODULE_URL=$(${GIT} remote get-url origin)
	if [[ $? -ne 0 ]]
	then
		errmsg "[E] failed to retrieve git submodule URL!"
		fail=1
	fi
	if ! cd "${INVOCATIONDIR}" 
	then
		errmsg "[E] failed to make return to directory '${INVOCATIONDIR}'!"
		fail=1
	fi
	if [[ $fail -ne 0 ]]
	then
		exit 1
	fi
	readonly SUBMODULE_URL
}

# 
# Create new project directory and initialize git repository with submodule
#
make_new_projectdir()
{
	if ! "${MKDIR}" -p "$1"
	then
		errmsg "[E] failed to create requested new project directory '$1'!"
      		exit 1	
	fi
	errmsg "[I] directory '$1' successfully created"
	if ! cd "$1"
	then
		errmsg "[I] failed to make '$1' the current directory!"
		exit 1
	fi
	if ! "${GIT}" init
	then
		errmsg "[E] Failed to initialize git repository in '$1'!"
		exit 1
	fi
	local submoduledir="${SUBMODULE_URL##*/}" 
	submoduledir="${submoduledir%.git}" 
	if ! "${GIT}" submodule add "${SUBMODULE_URL}" 
	then
		errmsg "[E] failed to add '${SUBMODULE_URL}' as a submodule to '$1'!"
		exit 1
	fi

	local fail=0
	if ! ${CP} "${submoduledir}/CMakeLists.txt.template" ./CMakeLists.txt 
	then
		fail=1
		errmsg "[E] failed to copy '${submoduledir}/CMakeLists.txt.template' to './CMakeLists.txt'!"
	fi
	if ! ${CP} "${submoduledir}/CMakePresets.json.template" ./CMakePresets.json
	then
		fail=1
		errmsg "[E] failed to copy '${submoduledir}/CMakePresets.json.template' to './CMakePresets.json'!"
	fi
	if ! ${CP} "${submoduledir}/config+build.sh" ./config+build.sh
	then
		fail=1
		errmsg "[E] failed to copy '${submoduledir}/config+build.sh' to './config+build.sh'!"
	fi
	if ! ${CP} "${submoduledir}/Doxyfile" ./Doxyfile
	then
		fail=1
		errmsg "[E] failed to copy '${submoduledir}/Doxyfile' to './Doxyfile'!"
	fi
	if [[ $fail -ne 0 ]]
	then
		exit 1
	fi

}


echo $PROGNAM
echo $SCRIPTDIR

echo $PWD
submodule_url
echo $SUBMODULE_URL

if [[ $# -ne 1 ]]
then
	errmsg "[E] no new project directory specified!"
	usage >&2
	exit 1
fi
if [[ -e "$1" ]]
then
	errmsg "[E] specified new project directory '$1' already exists!"
	exit 1
fi

make_new_projectdir "$1" 
