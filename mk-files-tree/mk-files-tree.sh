#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause

set -o pipefail

Seed=0
Depth=2
DirectoryChars=2
FileNameLength=250
NumberOfFiles=4096
FileSizeMin=0
FileSizeMax=32768
TextLineCharsMin=1
TextLineCharsMax=128
TextFile=
BaseDirectory=./test

while getopts "s:d:c:l:n:i:a:I:A:B:th" opt
do
	case "${opt}" in
	(s)
		Seed=${OPTARG}
		;;
	(d)
		Depth=${OPTARG}
		;;
	(c)
		DirectoryChars=${OPTARG}
		;;
	(l)
		FileNameLength=${OPTARG}
		;;
	(n)
		NumberOfFiles=$OPTARG
		;;
	(i)
		FileSizeMin=$OPTARG
		;;
	(a)
		FileSizeMax=$OPTARG
		;;
	(I)
		TextLineCharsMin=$OPTARG
		;;
	(A)
		TextLineCharsMax=$OPTARG
		;;
	(B)
		BaseDirectory="$OPTARG"
		;;
	(t)
		TextFile=yes
		;;
	(*)
		echo "$0: HELP: -s seed: random seed"
		echo "$0: HELP: -d depth: directory depth"
		echo "$0: HELP: -c characters: characters of directory name"
		echo "$0: HELP: -l length: File name length (text: The number of printable chars)"
		echo "$0: HELP: -n number_of_files: The number of files"
		echo "$0: HELP: -i file_size_min: Min file size to create"
		echo "$0: HELP: -a file_size_max: Max file size to create"
		echo "$0: HELP: -I text_line_min: Min text line characters"
		echo "$0: HELP: -A text_line_max: Max text line characters"
		echo "$0: HELP: -t: Create text file (other wise binary file)"
		echo "$0: HELP: -B: Base directory"
		exit 1
		;;
	esac
done

echo -n	"$0: INFO: Command line arguments. "
echo -n	"-s ${Seed} -d ${Depth} -c ${DirectoryChars} "
echo -n "-l ${FileNameLength} -n ${NumberOfFiles} -i ${FileSizeMin} -a ${FileSizeMax} "
echo -n "-I ${TextLineCharsMin} -A ${TextLineCharsMax} "
echo -n "-B \"${BaseDirectory}\" "
[[ -n "${TextFile}" ]] && echo "-t" || "echo"

# arg: length seed
function RandBase64Str() {
	local	result
	local	prand_bin_len

	prand_bin_len=$(( ( ( $1 + 3 ) / 4 ) * 3 ))
	../prand/prand -s $2 ${prand_bin_len} | base64 | tr '/' '_' | tr -d '\n' | cut -c 1-$1
	result=$?
	if (( ${result} != 0 ))
	then
		echo "$0.RandBase64Str: ERROR: May broken pipe. length=$1, seed=$2"
		return ${result}
	fi
	return 0
}

# arg: seed
function RandUint64() {
	local	result

	printf "%u" 0x$( ../prand/prand -s $1 8 | od -t x8 | head -1 | cut -d ' ' -f 2 )
	result=$?
	if (( ${result} != 0 ))
	then
		echo "$0.RandUint64: ERROR: May broken pipe. seed=$1"
		return ${result}
	fi
	return 0
}

# arg: seed
function RandUint64Hex() {
	local	result

	echo "0x$( ../prand/prand -s $1 8 | od -t x8 | head -1 | cut -d ' ' -f 2 )"
	result=$?
	if (( ${result} != 0 ))
	then
		echo "$0.RandUint64Hex: ERROR: May broken pipe. seed=$1"
		return ${result}
	fi
	return 0
}

# arg directory_path
function RemoveTrailingSlash() {
	local	result
	local	l
	local	m

	l=${#1}
	m=$(( ${l} - 1 ))

	if [[ "${1:${m}:1}" == "/" ]]
	then
		echo "${1:0:${m}}"
	else
		echo "${1}"
	fi
	result=$?
	if (( ${result} != 0 ))
	then
		echo "$0.RemoveTrailingSlash: ERROR: May echo failed. directory_path=$1"
		return ${result}
	fi
	return 0
}

# arg: file_name depth chars
function FileNameToDirectory() {
	local	result
	local	i
	local	x
	local	dir

	dir=""
	i=0
	while (( ${i} < $2 ))
	do
		x=$(( ${i} * $3 ))
		dir="${dir}/${1:$x:$3}"
		i=$(( ${i} + 1 ))
	done
	echo ${dir}
	result=$?
	if (( ${result} != 0 ))
	then
		echo "$0.RemoveTrailingSlash: ERROR: May echo failed. directory_path=$1"
		return ${result}
	fi
	return 0
}

if [[ "${BaseDirectory}" != "/" ]]
then
	BaseDirectory="$( RemoveTrailingSlash "${BaseDirectory}" )"
fi

file_num=0

while (( ${file_num} < ${NumberOfFiles} ))
do
	seed_num=$(( ${Seed} + ${file_num} ))
	file_name="$( RandBase64Str ${FileNameLength} ${seed_num} )"
	result=$?
	if (( ${result} != 0 ))
	then
		exit ${result}
	fi

	file_dir="${BaseDirectory}$( FileNameToDirectory ${file_name} ${Depth} ${DirectoryChars} )"
	result=$?
	if (( ${result} != 0 ))
	then
		exit ${result}
	fi

	file_path="${file_dir}/${file_name}"
	mkdir -p "${file_dir}"
	result=$?
	if (( ${result} != 0 ))
	then
		echo "$0.main: ERROR: Can not create or validate directory. file_dir=\"$1\""
		exit ${result}
	fi

	r64=$( RandUint64 ${seed_num} )
	result=$?
	if (( ${result} != 0 ))
	then
		exit ${result}
	fi

	file_size=$( calc "floor( ${FileSizeMin} + ( ${FileSizeMax} - ${FileSizeMin} + 1 ) * ( ${r64} / 18446744073709551616 ) )" )
	result=$?
	if (( ${result} != 0 ))
	then
		echo "$0.main: ERROR: calc failed."
		exit ${result}
	fi

	file_size=$( echo ${file_size} )
	if [[ -z "${TextFile}" ]]
	then
		echo "${file_path}: Create. file_num=${file_num}, size=${file_size}"
		../prand/prand -s ${seed_num} ${file_size} > "${file_path}"
		result=$?
		if (( ${result} != 0 ))
		then
			echo "$0.main: ERROR: prand exited with error. result=${result}"
			exit ${result}
		fi
	else
		echo "${file_path}: Create. file_num=${file_num}, size(text_chars)=${file_size}"
		RandBase64Str ${file_size} ${seed_num} | ../mashlf/mashlf -i ${TextLineCharsMin} -a ${TextLineCharsMax} -s ${seed_num} > "${file_path}"
		result=$?
		if (( ${result} != 0 ))
		then
			echo "$0.main: ERROR: Can not create text file. file_path=\"${file_path}\", result=${result}"
			exit ${result}
		fi
	fi
	file_num=$(( ${file_num} + 1 ))
done
exit 0
