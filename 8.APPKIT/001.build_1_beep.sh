#!/bin/bash
#Author: weike
#MyQQID: 2362317758

export TOPDIR=$PWD

export MODPKG_ROOT=${TOPDIR}/pkg
export MODSRC_ROOT=${TOPDIR}/src

export MODNAME=10_smsdemo

export MODPKG_NAME=${MODNAME}.tar.gz
export MODSRC_NAME=${MODNAME}

export MODPKG_PATH=${MODPKG_ROOT}/${MODPKG_NAME}
export MODSRC_PATH=${MODSRC_ROOT}/${MODSRC_NAME}
export MODOUT_PATH=$TOPDIR/output

export GCC_PATH_DEFAULT=$PWD/../0.GCCKIT/armgcc
export GCC_TYPE_DEFAULT=arm-none-linux-gnueabi

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
check_compiler()
{
	$GCC_TYPE-gcc -v > /dev/null  2>&1
	if [ $? -eq 0 ]; then
		pinfo "check global gcc type : $GCC_TYPE"
	else
		pinfo "check local gcc type : $GCC_PATH"
		if [ -d $GCC_PATH ]; then
			pdone "check local gcc valid OK!"
			export PATH=$GCC_PATH/bin:$PATH
			$GCC_TYPE-gcc -v > /dev/null  2>&1
			if [ $? -eq 0 ]; then
				pdone "test local gcc valid OK!"
			else
				perro "test local gcc valid Failed!"
			exit 1
			fi
		else
			perro "check local gcc valid Failed!"
			exit 1
		fi
	fi
}

check_env()
{
	if [ -z "$GCC_TYPE" ]; then
		pwarn "No compile env configured!"
		pwarn "Set default compile env"
		export GCC_PATH=$GCC_PATH_DEFAULT
		export GCC_TYPE=$GCC_TYPE_DEFAULT
		export HOST=$GCC_TYPE
		export TARGET=$GCC_TYPE
		export CC="${HOST}-gcc"
		export CXX="${HOST}-g++"
		export OBJC="${CC}"
		export LD="${HOST}-ld"
		export ARCH=arm
		export CROSS_COMPILE=${HOST}-
		pinfo "--------default compile env------"
		pinfo "ARCH : $ARCH"
		pinfo "HOST : $HOST"
		pinfo "GCC_TYPE : $GCC_TYPE"
		pinfo "GCC_PATH : $GCC_PATH"
		pinfo "CROSS_COMPILE: $CROSS_COMPILE"
		pinfo "---------------------------------"
		check_compiler
	else
		pwarn "Get compile env configured!"
		pinfo "--------current compile env------"
		pinfo "ARCH : $ARCH"
		pinfo "HOST : $HOST"
		pinfo "GCC_TYPE : $GCC_TYPE"
		pinfo "GCC_PATH : $GCC_PATH"
		pinfo "CROSS_COMPILE: $CROSS_COMPILE"
		pinfo "---------------------------------"
	fi
}
#-----------------------------------------------------------------------
check_source()
{
	if [ ! -d ${MODSRC_PATH} ]; then
		pwarn "No ${MODSRC_PATH} found, try extract new source!"
		if [ -f ${MODPKG_PATH} ]; then
			cd ${MODPKG_ROOT}
			if [ "${MODPKG_NAME##*.}"x = "gz"x ]; then
				tar xzf ${MODPKG_PATH} -C ${MODSRC_ROOT}
				if [ $? -eq 0 ]; then
					pdone "Extract .gz  package: ${MODPKG_NAME} OK!"
				else
					perro "Extract .gz  package: ${MODPKG_NAME} Failed!"
					cd -> /dev/null
					exit 1
				fi
			elif [ "${MODPKG_NAME##*.}"x = "bz2"x ]; then
				tar xjf ${MODPKG_PATH} -C ${MODSRC_ROOT}
				if [ $? -eq 0 ]; then
					pdone "Extract .bz2  package: ${MODPKG_NAME} OK!"
				else
					perro "Extract .bz2  package: ${MODPKG_NAME} Failed!"
					cd -> /dev/null
					exit 1
				fi
			else
				perro "Unknown tar file type!"
				cd -> /dev/null
				exit 1
			fi
			cd -> /dev/null
		else
			perro "No package: ${MODPKG_NAME} found!"
			exit 1
		fi
	else
		pdone "Check source OK!"
	fi
}

pack_module()
{
	if [ ! -d  ${MODSRC_PATH} ]; then
		perro "No MODSRC_PATH: ${MODSRC_PATH}"
		exit 1
	fi
	cd ${MODSRC_ROOT}
	tar czvf  ./${MODPKG_NAME} ${MODSRC_NAME}
	if [ $? -eq 0 ];then
		pdone "Tar source OK!"
	else
		perro "Tar source Failed!"
		exit 1
	fi
	mv ./${MODPKG_NAME} ${MODPKG_PATH}
	if [ $? -eq 0 ];then
		pdone "Pack source OK!"
		cd - > /dev/null
	else
		perro "Pack source Failed!"
		cd - > /dev/null
		exit 1
	fi
}

build_module()
{
	cd ${MODSRC_PATH}
	make && make install
    [ $? -ne 0 ] && perro "Build module Failed!" && exit 1
	pdone "Build module OK!"
	cd - > /dev/null
}

clean_module()
{
	cd ${MODSRC_PATH}
	if [ -f Makefile ]; then
		make distclean
		find ./ -name "*.log" | xargs rm -rf
	fi
	pdone "Clean module OK!"
	cd - > /dev/null
}

if [ "x$1" = "xbuild" ]; then
	check_env
	export CFLAGS="-I${MODSRC_PATH} -I${MODOUT_PATH}/include" 
	export LDFLAGS="-L${MODOUT_PATH}/lib"
	export PKG_CONFIG_PATH=${MODOUT_PATH}/lib/pkgconfig
	check_source

	if [ ! -d ${MODOUT_PATH} ]; then
		mkdir  -p ${MODOUT_PATH}
	fi
	export PREFIX=${MODOUT_PATH}

	build_module
elif [ "x$1" = "xclean" ]; then
    clean_module
elif [ "x$1" = "xpack" ]; then
	pack_module
else
	pinfo "Input: $0 build "
	pinfo "Input: $0 clean "
	pinfo "Input: $0 pack"
fi
