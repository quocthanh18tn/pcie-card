#
# This file is the pcietool recipe.
#

SUMMARY = "Simple pcietool application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://selftest \
				file://gpio-test \
				file://i2c-test.c \
				file://spi-test.c \
				file://Makefile \
				file://pcietool \
	"

S = "${WORKDIR}"
RDEPENDS_${PN} += "bash"

do_compile(){
	oe_runmake
}

do_install() {
	     install -d ${D}/${bindir}
	     install -m 0755 ${S}/selftest ${D}/${bindir}
	     install -m 0755 ${S}/i2c-test ${D}/${bindir}
	     install -m 0755 ${S}/spi-test ${D}/${bindir}
	     install -m 0755 ${S}/gpio-test ${D}/${bindir}
	     install -m 0755 ${S}/pcietool ${D}/${bindir}
}
