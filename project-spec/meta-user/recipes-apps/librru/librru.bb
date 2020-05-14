#
# This file is the librru recipe.
#

SUMMARY = "Simple librru application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"


FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = "file://*"

S = "${WORKDIR}"
DEPENDS += " libiio"

PACKAGE_ARCH = "${MACHINE_ARCH}"
PROVIDES = "rru"
TARGET_CC_ARCH += "${LDFLAGS}"

do_compile[nostamp] = "1"
do_build[nostamp] = "1"
do_install[nostamp] = "1"

do_compile() {
	oe_runmake
}
do_install() {
    install -d ${D}${libdir}
    install -d ${D}${includedir}
    oe_libinstall -so librru ${D}${libdir}
    install -d -m 0755 ${D}${includedir}
    install -m 0755 ${S}/*.h ${D}${includedir}
    install -m 0755 ${S}/*/*.h ${D}${includedir}
}
 
FILES_${PN} = "${libdir}/*.so.* ${includedir}/*"
FILES_${PN}-dev = "${libdir}/*.so"
