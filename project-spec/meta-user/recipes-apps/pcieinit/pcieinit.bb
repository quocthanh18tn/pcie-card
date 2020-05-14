#
# This file is the pcieinit recipe.
#

SUMMARY = "Simple pcieinit application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://pcieinit \
		   file://lmkconfig \
		   file://Makefile \
		   file://rrud \
"
S = "${WORKDIR}"
RDEPENDS_${PN} += "bash"


FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

inherit update-rc.d

INITSCRIPT_NAME = "pcieinit"

INITSCRIPT_PARAMS = "start 99 S ."

do_install() {
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${S}/pcieinit ${D}${sysconfdir}/init.d/pcieinit
	install -d ${D}/${bindir}
	install -m 0755 ${S}/lmkconfig ${D}/${bindir}
	install -m 0755 ${S}/rrud ${D}/${bindir}
}
FILES_${PN} += "${sysconfdir}/*"