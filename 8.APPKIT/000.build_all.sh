#!/bin/bash

#-----------------------------------------------------------------------
perro()
{
    echo -e "\033[47;41mERROR: $*\033[0m"
}

pwarn()
{
    echo -e "\033[47;43mWARN: $*\033[0m"
}

pinfo()
{
    echo -e "\033[47;44mINFO: $*\033[0m"
}

pdone()
{
    echo -e "\033[47;42mINFO: $*\033[0m"
}

#-----------------------------------------------------------------------

if [ "x$1" = "xbuild" ]; then
	export cmd=build
elif [ "x$1" = "xclean" ]; then
	export cmd=clean
elif [ "x$1" = "xpack" ]; then
	export cmd=pack
else
	pinfo "Input: $0 build "
	pinfo "Input: $0 clean "
	pinfo "Input: $0 pack"
	exit 1
fi
for file in $(ls *.sh | grep -v 000.build_all.sh)
do
	./$file $cmd
done
