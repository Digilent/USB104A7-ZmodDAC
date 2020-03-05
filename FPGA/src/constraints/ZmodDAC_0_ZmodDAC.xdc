################# Constraints for the USB104A7's Zmod Port #################

set_property -dict { PACKAGE_PIN F18 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[0] }]; #IO_L22N_T3_A16_15 Sch=syzygy_s[19]
set_property -dict { PACKAGE_PIN G18 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[1] }]; #IO_L22P_T3_A17_15 Sch=syzygy_s[18]
set_property -dict { PACKAGE_PIN C17 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[2] }]; #IO_L20N_T3_A19_15 Sch=syzygy_d_n[6]
set_property -dict { PACKAGE_PIN J17 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[3] }]; #IO_L23P_T3_FOE_B_15 Sch=syzygy_s[20]
set_property -dict { PACKAGE_PIN J14 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[4] }]; #IO_L19P_T3_A22_15 Sch=syzygy_s[16]
set_property -dict { PACKAGE_PIN H15 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[5] }]; #IO_L19N_T3_A21_VREF_15 Sch=syzygy_s[17]
set_property -dict { PACKAGE_PIN D18 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[6] }]; #IO_L21N_T3_DQS_A18_15 Sch=syzygy_d_n[7]
set_property -dict { PACKAGE_PIN E18 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[7] }]; #IO_L21P_T3_DQS_15 Sch=syzygy_d_p[7]
set_property -dict { PACKAGE_PIN C15 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[8] }]; #IO_L12N_T1_MRCC_15 Sch=syzygy_d_n[5]
set_property -dict { PACKAGE_PIN D15 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[9] }]; #IO_L12P_T1_MRCC_15 Sch=syzygy_d_p[5]
set_property -dict { PACKAGE_PIN A18 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[10] }]; #IO_L10N_T1_AD11N_15 Sch=syzygy_d_n[3]
set_property -dict { PACKAGE_PIN B18 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[11] }]; #IO_L10P_T1_AD11P_15 Sch=syzygy_d_p[3]
set_property -dict { PACKAGE_PIN B17 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[12] }]; #IO_L7N_T1_AD2N_15 Sch=syzygy_d_n[1]
set_property -dict { PACKAGE_PIN B16 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_DATA_0[13] }]; #IO_L7P_T1_AD2P_15 Sch=syzygy_d_p[1]

set_property -dict { PACKAGE_PIN F15 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_CLKIN_0 }]; #IO_L14P_T2_SRCC_15 Sch=syzygy_c2p_clk_p
set_property -dict { PACKAGE_PIN H16 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_CLKIO_0 }]; #IO_L13P_T2_MRCC_15 Sch=syzygy_p2c_clk_p


#DAC SPI
set_property -dict { PACKAGE_PIN E15 IOSTANDARD LVCMOS18  DRIVE 4} [get_ports { ZmodDAC_0_DAC_SDIO_0 }]; #IO_L11P_T1_SRCC_15 Sch=syzygy_d_p[4]
set_property -dict { PACKAGE_PIN E16 IOSTANDARD LVCMOS18 DRIVE 4} [get_ports { ZmodDAC_0_DAC_CS_0 }]; #IO_L11N_T1_SRCC_15 Sch=syzygy_d_n[4]
set_property -dict { PACKAGE_PIN A16 IOSTANDARD LVCMOS18  DRIVE 4} [get_ports { ZmodDAC_0_DAC_SCLK_0 }]; #IO_L8N_T1_AD10N_15 Sch=syzygy_d_n[2]


set_property -dict { PACKAGE_PIN A13 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_SET_FS1_0 }]; #IO_L9P_T1_DQS_AD3P_15 Sch=syzygy_d_p[0]
set_property -dict { PACKAGE_PIN A14 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_SET_FS2_0 }]; #IO_L9N_T1_DQS_AD3N_15 Sch=syzygy_d_n[0]
set_property -dict { PACKAGE_PIN A15 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_RESET_0 }]; #IO_L8P_T1_AD10P_15 Sch=syzygy_d_p[2]
set_property -dict { PACKAGE_PIN C16 IOSTANDARD LVCMOS18 } [get_ports { ZmodDAC_0_DAC_EN_0 }]; #IO_L20P_T3_A20_15 Sch=syzygy_d_p[6]



create_generated_clock -name ZmodDAC_0_DAC_CLKIN_0 -source [get_pins design_1_i/ZmodDAC_0/ZmodDAC1411_Controll_0/U0/InstDAC_ClkinODDR/C] -divide_by 1 [get_ports ZmodDAC_0_DAC_CLKIN_0]
create_generated_clock -name ZmodDAC_0_DAC_CLKIO_0 -source [get_pins design_1_i/ZmodDAC_0/ZmodDAC1411_Controll_0/U0/InstDAC_ClkIO_ODDR/C] -divide_by 1 [get_ports ZmodDAC_0_DAC_CLKIO_0]
set_output_delay -clock [get_clocks ZmodDAC_0_DAC_CLKIN_0] -clock_fall -min -add_delay -1.200 [get_ports {ZmodDAC_0_DAC_DATA_0[*]}]
set_output_delay -clock [get_clocks ZmodDAC_0_DAC_CLKIN_0] -clock_fall -max -add_delay 0.250 [get_ports {ZmodDAC_0_DAC_DATA_0[*]}]
set_output_delay -clock [get_clocks ZmodDAC_0_DAC_CLKIN_0] -min -add_delay -1.100 [get_ports {ZmodDAC_0_DAC_DATA_0[*]}]
set_output_delay -clock [get_clocks ZmodDAC_0_DAC_CLKIN_0] -max -add_delay 0.130 [get_ports {ZmodDAC_0_DAC_DATA_0[*]}]

