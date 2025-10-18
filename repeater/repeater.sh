#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause

set -o pipefail

result=0

time_marker=$( date +%y%m%d%H%M%S )
cmd_body=${1##*/}
cmd_body=${cmd_body%.*}
prefix="repeater_${cmd_body}_${time_marker}_"
i=0
while (( ${i} <= 9999 ))
do
	echo "$0: Try. i=${i}"

	now_date=$( date +%y%m%d%H%M%S )
	log_file="${prefix}$( printf "%04d" ${i} )_${now_date}.log"
	"$@"  2>&1 | tee "${log_file}"
	result=$?
	if (( ${result} == 0 ))
	then
		echo "$0: Success. i=${i}"
		break
	fi
	i=$(( ${i} + 1 ))
done
exit ${result}
