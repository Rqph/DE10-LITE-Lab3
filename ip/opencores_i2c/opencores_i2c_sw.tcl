#
# opencores_i2c_sw.tcl
#

# Create a new driver
create_driver opencores_i2c

# Associate it with some hardware known as "videosampler"
set_sw_property hw_class_name opencores_i2c

# The version of this driver
set_sw_property version 9.1

# This driver may be incompatible with versions of hardware less
# than specified below. Updates to hardware and device drivers
# rendering the driver incompatible with older versions of
# hardware are noted with this property assignment.
#
# Multiple-Version compatibility was introduced in version 7.1;
# prior versions are therefore excluded.
set_sw_property min_compatible_hw_version 1.1

# Initialize the driver in alt_sys_init()
set_sw_property auto_initialize false

# Location in generated BSP that above sources will be copied into
set_sw_property bsp_subdirectory drivers

# Interrupt properties: This driver supports both legacy and enhanced
# interrupt APIs, as well as ISR preemption.
#set_sw_property isr_preemption_supported true
#set_sw_property supported_interrupt_apis "legacy_interrupt_api enhanced_interrupt_api"

#
# Source file listings...
#

# C/C++ source files
add_sw_property c_source HAL/src/opencores_i2c.c


# Include files
add_sw_property include_source HAL/inc/opencores_i2c.h
add_sw_property include_source inc/opencores_i2c_regs.h

# This driver supports HAL & UCOSII BSP (OS) types
add_sw_property supported_bsp_type HAL
add_sw_property supported_bsp_type ERIKA
add_sw_property supported_bsp_type CONTIKI

# End of file
