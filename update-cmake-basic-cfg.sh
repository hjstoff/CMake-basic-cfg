#! /bin/bash
set -u
readonly PROGNAM=${BASH_SOURCE[0]##/}"
errmsg()
{
	echo "${PROGNAM}: $1" >&2
}

errmsg " Stub program!"
