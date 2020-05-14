FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
 
XSCTH_BUILD_DEBUG = "1"
#YAML_COMPILER_FLAGS_append = " -DXPS_BOARD_ZCU102"

do_compile_prepend(){
 	install -m 0644 ${TOPDIR}/../project-spec/hw-description/psu_init.c ${B}/fsbl/psu_init.c 
 	install -m 0644 ${TOPDIR}/../project-spec/hw-description/psu_init.h ${B}/fsbl/psu_init.h 
}