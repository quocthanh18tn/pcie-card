
do_configure[depends] += "kern-tools-native:do_populate_sysroot"
KERNEL_CONFIG_COMMAND = ""
SRC_URI += "file://plnx_kernel.cfg"
RDEPENDS_kernel-base = ""
FILESEXTRAPATHS_prepend := "${THISDIR}/configs:"
KERNEL_IMAGETYPE_zynq ?= "zImage"
do_configure_append () {
	merge_config.sh -m ${B}/.config ${@" ".join(find_sccs(d))}
	oe_runmake -C ${S} O=${B} oldconfig
}
do_deploy_append () {
	install -m 0644 ${D}/boot/System.map-${KERNEL_VERSION} ${DEPLOYDIR}/System.map.linux
}
