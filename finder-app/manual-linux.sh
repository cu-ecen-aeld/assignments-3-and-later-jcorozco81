#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here  
    echo "Make: mrproper"
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE mrproper

    echo "Make: defconfig"
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE defconfig

    echo "Make: all"
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE -j$(nproc) all
    
    # skipped modules install
    # make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE modules
    
    echo "Make: dtbs"
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE dtbs
    # TODO
fi

echo "Adding the Image in outdir"
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}


echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
cd "$OUTDIR"
mkdir rootfs
cd rootfs/
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log
# TODO

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
make distclean
make defconfig
    # TODO

else
    cd busybox
fi

# TODO: Make and install busybox

make -j$(nproc) ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE install
cd $OUTDIR/rootfs/

# TODO

echo "Library dependencies"

echo "--Program Interpreter--"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"

echo "--Shared Library--"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

echo "--Needed bin/busybox--"
${CROSS_COMPILE}readelf -a bin/busybox | grep "NEEDED"

echo "--Needed bin/sh--"
${CROSS_COMPILE}readelf -a bin/sh | grep "NEEDED"


# TODO: Add library dependencies to rootfs
SYSROOT=$(aarch64-none-linux-gnu-gcc -print-sysroot)
# SYSROOT="/home/jcorozco81/toolchains/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/"

${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter" | awk '{print $NF}' | tr -d '[]' | while read -r line; do
FILE_NAME=$(basename "$line")
echo $FILE_NAME
find $SYSROOT -name $FILE_NAME -type f 2>/dev/null | while read -r file_path; do 
echo $file_path
cp -r ${file_path} ${OUTDIR}/rootfs/lib
echo $?
echo "Copied $FILE_NAME from $file_path to $OUTDIR/rootfs/lib"
done
done


${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library" | awk '{print $NF}' | tr -d '[]' | while read -r lib_name; do
echo $lib_name
find $SYSROOT -name $lib_name -type f 2>/dev/null | while read -r file_path; do 
cp -r ${file_path} ${OUTDIR}/rootfs/lib64
echo $?
echo "Copied $lib_name from $file_path to $OUTDIR/rootfs/lib64"
done
done



# TODO: Make device nodes

sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 3

# TODO: Clean and build the writer utility
cd $FINDER_APP_DIR

make writer clean CC=aarch64-none-linux-gnu-gcc



# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cd $FINDER_APP_DIR
cp writer ${OUTDIR}/rootfs/home
cp finder.sh ${OUTDIR}/rootfs/home
cp finder-test.sh ${OUTDIR}/rootfs/home
cp autorun-qemu.sh ${OUTDIR}/rootfs/home

cd ${OUTDIR}/rootfs/home
# make writer CROSS_COMPILE=aarch64-none-linux-gnu-
sed -i 's/\.\.\/conf/conf/g' finder-test.sh

mkdir conf

cp $FINDER_APP_DIR/conf/username.txt ${OUTDIR}/rootfs/home/conf
cp $FINDER_APP_DIR/conf/assignment.txt ${OUTDIR}/rootfs/home/conf


# TODO: Chown the root directory
cd $OUTDIR/rootfs/
sudo chown -R root:root *
# TODO: Create initramfs.cpio.gz

cd $OUTDIR/rootfs/
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
gzip -f ${OUTDIR}/initramfs.cpio