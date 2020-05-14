SUMMARY = "Analog Devices Industrial IO library"
LICENSE = "LGPLv2.1"
LIC_FILES_CHKSUM = "file://COPYING.txt;md5=7c13b3376cea0ce68d2d2da0a1b3a72c"
DEPENDS = "avahi bison flex libxml2 ncurses"


FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = "file://*"
S = "${WORKDIR}/libiio"

inherit cmake

PACKAGES =+ "iiod-dbg iiod iio-utils-dbg iio-utils iio-tests"

FILES_iiod = "${sbindir}/iiod"
FILES_iio-utils = "${bindir}"
FILES_iio-tests = "${bindir}"
FILES_iio-utils-dbg = "${bindir}/.debug"

