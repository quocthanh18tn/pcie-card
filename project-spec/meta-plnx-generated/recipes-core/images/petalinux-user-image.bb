DESCRIPTION = "PETALINUX image definition for Xilinx boards"
LICENSE = "MIT"

require recipes-core/images/petalinux-image-common.inc 

inherit extrausers 
COMMON_FEATURES = "\
		ssh-server-dropbear \
		hwcodecs \
		"
IMAGE_LINGUAS = " "

IMAGE_INSTALL = "\
		kernel-modules \
		i2c-tools \
		i2c-tools-dev \
		i2c-tools-misc \
		i2c-tools-dbg \
		mtd-utils \
		util-linux-fsck \
		util-linux-umount \
		util-linux-mount \
		util-linux-fdisk \
		util-linux-lscpu \
		canutils \
		openssh-sftp-server \
		pciutils \
		python \
		python-io \
		python-shell \
		python-ctypes \
		run-postinsts \
		libmetal \
		libmetal-dev \
		libmetal-dbg \
		libmetal-demos \
		udev-extraconf \
		packagegroup-core-boot \
		packagegroup-core-ssh-dropbear \
		libsysfs \
		tcf-agent \
		watchdog-init \
		bridge-utils \
		tcpdump \
		tcpdump-dev \
		tcpdump-dbg \
		hellopm \
		libiio \
		librru \
		peekpoke \
		rruinit \
		rrupy \
		rrutool \
		"
EXTRA_USERS_PARAMS = "usermod -P root root;"
