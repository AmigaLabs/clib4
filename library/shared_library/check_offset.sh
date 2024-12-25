#!/bin/bash --

set -u

# If any argument is provided to this script, then it provides the
# current largest offset

offsets=()
# 1. Initially, grab all the files and their offsets. There is no
#    adherence to alphabetical ordering of file names and offset
#    increment.
for stub in $(find -name "stubs_*.c" -and -not -name "stubs_common.c")
do
    stub_offset=$(sed -n "s/Clib4Call(.*, \([0-9]\+\));/\1/p" "$stub")
    if [[ -z "$stub_offset" ]]
    then
	echo >&2 "Did not expect \"$stub\" to not have an offset"
	exit 1
    else
	offsets+=( $stub_offset )
    fi
done

# 2. Sort the offsets
sorted_offsets=($(printf '%s\n' "${offsets[@]}" | sort -n))

# 3.a If an argument was given, display the larget offset
if [[ $# -gt 0 ]]
then
    echo "The largest offset is \"${sorted_offsets[-1]}\""
    exit 0
# 3.b Otherwise, check that all offsets are an incremement of 4
else
    prev_offset=${sorted_offsets[0]}
    for (( i=1; i<${#sorted_offsets[@]}; i++ ))
    do
	if (( prev_offset + 4 != ${sorted_offsets[i]} ))
	then
	    echo >&2 "Expected offset of increment 4, but found previous" \
"offset $prev_offset versus offset ${sorted_offsets[i]}"
	    exit 2
	fi
	prev_offset=${sorted_offsets[i]}
    done
fi

exit 0
