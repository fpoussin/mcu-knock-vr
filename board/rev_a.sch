<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.2.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="50" name="dxf" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
<layer number="100" name="Muster" color="7" fill="1" visible="no" active="no"/>
<layer number="101" name="Patch_Top" color="12" fill="4" visible="yes" active="yes"/>
<layer number="116" name="Patch_BOT" color="9" fill="4" visible="yes" active="yes"/>
<layer number="200" name="200bmp" color="1" fill="10" visible="no" active="no"/>
<layer number="250" name="Descript" color="3" fill="1" visible="no" active="no"/>
<layer number="251" name="SMDround" color="12" fill="11" visible="no" active="no"/>
<layer number="254" name="cooling" color="7" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S" xrefpart="/%S.%C%R">
<libraries>
<library name="stm32">
<description>ST Cortex-M3 STM32F101/103 processor family  in LQFP100, 64, 48 and VFQPFN36 pin versions</description>
<packages>
<package name="LQFP48_2" urn="urn:adsk.eagle:footprint:7143670/1" locally_modified="yes">
<description>48-QFP, 0.50 mm pitch, 9.00 mm span, 7.00 X 7.00 X 1.60 mm body
&lt;p&gt;48-pin QFP package with 0.50 mm pitch, 9.00 mm lead span1 X 9.00 mm lead span2 with body size 7.00 X 7.00 X 1.60 mm&lt;/p&gt;</description>
<circle x="-4.2788" y="3.394" radius="0.25" width="0" layer="21"/>
<wire x1="-3.6" y1="3.144" x2="-3.6" y2="3.6" width="0.12" layer="21"/>
<wire x1="-3.6" y1="3.6" x2="-3.144" y2="3.6" width="0.12" layer="21"/>
<wire x1="3.6" y1="3.144" x2="3.6" y2="3.6" width="0.12" layer="21"/>
<wire x1="3.6" y1="3.6" x2="3.144" y2="3.6" width="0.12" layer="21"/>
<wire x1="3.6" y1="-3.144" x2="3.6" y2="-3.6" width="0.12" layer="21"/>
<wire x1="3.6" y1="-3.6" x2="3.144" y2="-3.6" width="0.12" layer="21"/>
<wire x1="-3.6" y1="-3.144" x2="-3.6" y2="-3.6" width="0.12" layer="21"/>
<wire x1="-3.6" y1="-3.6" x2="-3.144" y2="-3.6" width="0.12" layer="21"/>
<wire x1="3.6" y1="-3.6" x2="-3.6" y2="-3.6" width="0.12" layer="51"/>
<wire x1="-3.6" y1="-3.6" x2="-3.6" y2="3.6" width="0.12" layer="51"/>
<wire x1="-3.6" y1="3.6" x2="3.6" y2="3.6" width="0.12" layer="51"/>
<wire x1="3.6" y1="3.6" x2="3.6" y2="-3.6" width="0.12" layer="51"/>
<smd name="1" x="-4.1783" y="2.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="2" x="-4.1783" y="2.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="3" x="-4.1783" y="1.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="4" x="-4.1783" y="1.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="5" x="-4.1783" y="0.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="6" x="-4.1783" y="0.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="7" x="-4.1783" y="-0.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="8" x="-4.1783" y="-0.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="9" x="-4.1783" y="-1.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="10" x="-4.1783" y="-1.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="11" x="-4.1783" y="-2.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="12" x="-4.1783" y="-2.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="13" x="-2.75" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="14" x="-2.25" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="15" x="-1.75" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="16" x="-1.25" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="17" x="-0.75" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="18" x="-0.25" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="19" x="0.25" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="20" x="0.75" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="21" x="1.25" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="22" x="1.75" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="23" x="2.25" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="24" x="2.75" y="-4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="25" x="4.1783" y="-2.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="26" x="4.1783" y="-2.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="27" x="4.1783" y="-1.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="28" x="4.1783" y="-1.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="29" x="4.1783" y="-0.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="30" x="4.1783" y="-0.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="31" x="4.1783" y="0.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="32" x="4.1783" y="0.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="33" x="4.1783" y="1.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="34" x="4.1783" y="1.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="35" x="4.1783" y="2.25" dx="1.5588" dy="0.28" layer="1"/>
<smd name="36" x="4.1783" y="2.75" dx="1.5588" dy="0.28" layer="1"/>
<smd name="37" x="2.75" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="38" x="2.25" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="39" x="1.75" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="40" x="1.25" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="41" x="0.75" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="42" x="0.25" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="43" x="-0.25" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="44" x="-0.75" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="45" x="-1.25" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="46" x="-1.75" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="47" x="-2.25" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<smd name="48" x="-2.75" y="4.1783" dx="1.5588" dy="0.28" layer="1" rot="R90"/>
<text x="0" y="5.5927" size="1.27" layer="25" align="bottom-center">&gt;NAME</text>
<text x="0" y="-5.5927" size="1.27" layer="27" align="top-center">&gt;VALUE</text>
</package>
</packages>
<packages3d>
<package3d name="QFP50P900X900X160-48" urn="urn:adsk.eagle:package:7143663/1" locally_modified="yes" type="model">
<description>48-QFP, 0.50 mm pitch, 9.00 mm span, 7.00 X 7.00 X 1.60 mm body
&lt;p&gt;48-pin QFP package with 0.50 mm pitch, 9.00 mm lead span1 X 9.00 mm lead span2 with body size 7.00 X 7.00 X 1.60 mm&lt;/p&gt;</description>
<packageinstances>
<packageinstance name="LQFP48_2"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="PORT_A_L">
<wire x1="7.62" y1="10.16" x2="-2.54" y2="10.16" width="0.254" layer="94"/>
<wire x1="-2.54" y1="10.16" x2="-2.54" y2="-12.7" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-12.7" x2="7.62" y2="-12.7" width="0.254" layer="94"/>
<text x="-2.54" y="10.16" size="1.27" layer="95">&gt;NAME</text>
<pin name="PA0" x="-7.62" y="7.62" length="middle"/>
<pin name="PA1" x="-7.62" y="5.08" length="middle"/>
<pin name="PA2" x="-7.62" y="2.54" length="middle"/>
<pin name="PA3" x="-7.62" y="0" length="middle"/>
<pin name="PA4" x="-7.62" y="-2.54" length="middle"/>
<pin name="PA5" x="-7.62" y="-5.08" length="middle"/>
<pin name="PA6" x="-7.62" y="-7.62" length="middle"/>
<pin name="PA7" x="-7.62" y="-10.16" length="middle"/>
</symbol>
<symbol name="PORT_A_H">
<wire x1="7.62" y1="12.7" x2="-2.54" y2="12.7" width="0.254" layer="94"/>
<wire x1="-2.54" y1="12.7" x2="-2.54" y2="-10.16" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-10.16" x2="7.62" y2="-10.16" width="0.254" layer="94"/>
<text x="-2.54" y="12.7" size="1.27" layer="95">&gt;NAME</text>
<pin name="PA8" x="-7.62" y="10.16" length="middle"/>
<pin name="PA9" x="-7.62" y="7.62" length="middle"/>
<pin name="PA10" x="-7.62" y="5.08" length="middle"/>
<pin name="PA11" x="-7.62" y="2.54" length="middle"/>
<pin name="PA12" x="-7.62" y="0" length="middle"/>
<pin name="PA13" x="-7.62" y="-2.54" length="middle"/>
<pin name="PA14" x="-7.62" y="-5.08" length="middle"/>
<pin name="PA15" x="-7.62" y="-7.62" length="middle"/>
</symbol>
<symbol name="PORT_B_L">
<wire x1="-5.08" y1="12.7" x2="5.08" y2="12.7" width="0.254" layer="94"/>
<wire x1="5.08" y1="12.7" x2="5.08" y2="-10.16" width="0.254" layer="94"/>
<wire x1="5.08" y1="-10.16" x2="-5.08" y2="-10.16" width="0.254" layer="94"/>
<text x="-5.08" y="12.7" size="1.27" layer="95">&gt;NAME</text>
<pin name="PB0" x="10.16" y="10.16" length="middle" rot="R180"/>
<pin name="PB1" x="10.16" y="7.62" length="middle" rot="R180"/>
<pin name="PB2" x="10.16" y="5.08" length="middle" rot="R180"/>
<pin name="PB3" x="10.16" y="2.54" length="middle" rot="R180"/>
<pin name="PB4" x="10.16" y="0" length="middle" rot="R180"/>
<pin name="PB5" x="10.16" y="-2.54" length="middle" rot="R180"/>
<pin name="PB6" x="10.16" y="-5.08" length="middle" rot="R180"/>
<pin name="PB7" x="10.16" y="-7.62" length="middle" rot="R180"/>
</symbol>
<symbol name="PORT_B_H">
<wire x1="-5.08" y1="10.16" x2="5.08" y2="10.16" width="0.254" layer="94"/>
<wire x1="5.08" y1="10.16" x2="5.08" y2="-12.7" width="0.254" layer="94"/>
<wire x1="5.08" y1="-12.7" x2="-5.08" y2="-12.7" width="0.254" layer="94"/>
<text x="-5.08" y="10.16" size="1.27" layer="95">&gt;NAME</text>
<pin name="PB8" x="10.16" y="7.62" length="middle" rot="R180"/>
<pin name="PB9" x="10.16" y="5.08" length="middle" rot="R180"/>
<pin name="PB10" x="10.16" y="2.54" length="middle" rot="R180"/>
<pin name="PB11" x="10.16" y="0" length="middle" rot="R180"/>
<pin name="PB12" x="10.16" y="-2.54" length="middle" rot="R180"/>
<pin name="PB13" x="10.16" y="-5.08" length="middle" rot="R180"/>
<pin name="PB14" x="10.16" y="-7.62" length="middle" rot="R180"/>
<pin name="PB15" x="10.16" y="-10.16" length="middle" rot="R180"/>
</symbol>
<symbol name="VCC_VDD_S">
<wire x1="5.08" y1="17.78" x2="-5.08" y2="17.78" width="0.254" layer="94"/>
<wire x1="-5.08" y1="-17.78" x2="5.08" y2="-17.78" width="0.254" layer="94"/>
<wire x1="-5.08" y1="17.78" x2="-5.08" y2="-17.78" width="0.254" layer="94"/>
<text x="-5.08" y="17.78" size="1.27" layer="95">&gt;NAME</text>
<pin name="VDD_1" x="-10.16" y="15.24" length="middle" direction="pwr"/>
<pin name="VDD_2" x="-10.16" y="10.16" length="middle" direction="pwr"/>
<pin name="VDD_3" x="-10.16" y="5.08" length="middle" direction="pwr"/>
<pin name="VSS_1" x="-10.16" y="-5.08" length="middle" direction="pwr"/>
<pin name="VSS_2" x="-10.16" y="-10.16" length="middle" direction="pwr"/>
<pin name="VSS_3" x="-10.16" y="-15.24" length="middle" direction="pwr"/>
</symbol>
<symbol name="PORT_C_3">
<wire x1="7.62" y1="5.08" x2="-2.54" y2="5.08" width="0.254" layer="94"/>
<wire x1="-2.54" y1="5.08" x2="-2.54" y2="-5.08" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-5.08" x2="7.62" y2="-5.08" width="0.254" layer="94"/>
<text x="-2.54" y="5.08" size="1.27" layer="95">&gt;NAME</text>
<pin name="PC13" x="-7.62" y="2.54" length="middle"/>
<pin name="PC14" x="-7.62" y="0" length="middle"/>
<pin name="PC15" x="-7.62" y="-2.54" length="middle"/>
</symbol>
<symbol name="PORT_D_2">
<wire x1="-7.62" y1="2.54" x2="2.54" y2="2.54" width="0.254" layer="94"/>
<wire x1="2.54" y1="2.54" x2="2.54" y2="-5.08" width="0.254" layer="94"/>
<wire x1="2.54" y1="-5.08" x2="-7.62" y2="-5.08" width="0.254" layer="94"/>
<text x="-7.62" y="2.54" size="1.27" layer="95">&gt;NAME</text>
<pin name="PD0/OSC&lt;=" x="7.62" y="0" length="middle" rot="R180"/>
<pin name="PD1/OSC=&gt;" x="7.62" y="-2.54" length="middle" rot="R180"/>
</symbol>
<symbol name="BAT_AN_M">
<wire x1="-5.08" y1="7.62" x2="5.08" y2="7.62" width="0.254" layer="94"/>
<wire x1="5.08" y1="7.62" x2="5.08" y2="-7.62" width="0.254" layer="94"/>
<wire x1="5.08" y1="-7.62" x2="-5.08" y2="-7.62" width="0.254" layer="94"/>
<text x="-5.08" y="7.62" size="1.27" layer="95">&gt;NAME</text>
<pin name="VBAT" x="10.16" y="5.08" length="middle" direction="pwr" rot="R180"/>
<pin name="VSSA" x="10.16" y="0" length="middle" direction="pwr" rot="R180"/>
<pin name="VDDA" x="10.16" y="-5.08" length="middle" direction="pwr" rot="R180"/>
</symbol>
<symbol name="BOOT_RST">
<wire x1="-5.08" y1="5.08" x2="5.08" y2="5.08" width="0.254" layer="94"/>
<wire x1="5.08" y1="5.08" x2="5.08" y2="-5.08" width="0.254" layer="94"/>
<wire x1="5.08" y1="-5.08" x2="-5.08" y2="-5.08" width="0.254" layer="94"/>
<text x="-5.08" y="-7.62" size="1.27" layer="95">&gt;VALUE</text>
<text x="-5.08" y="5.08" size="1.27" layer="95">&gt;NAME</text>
<pin name="NRST" x="10.16" y="2.54" length="middle" direction="in" function="dot" rot="R180"/>
<pin name="BOOT0" x="10.16" y="-2.54" length="middle" direction="in" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="STM32F10XCXT6" prefix="IC">
<description>ST STM32F101/103 48pin LQFP</description>
<gates>
<gate name="PORTA_L" symbol="PORT_A_L" x="-12.7" y="38.1"/>
<gate name="PORTA_H" symbol="PORT_A_H" x="-12.7" y="10.16"/>
<gate name="PORTB_L" symbol="PORT_B_L" x="12.7" y="35.56"/>
<gate name="PORTB_H" symbol="PORT_B_H" x="12.7" y="12.7"/>
<gate name="POWER" symbol="VCC_VDD_S" x="-10.16" y="-20.32"/>
<gate name="PORTC" symbol="PORT_C_3" x="-12.7" y="-45.72"/>
<gate name="PORTD/OSC" symbol="PORT_D_2" x="15.24" y="-5.08"/>
<gate name="AN_POWER" symbol="BAT_AN_M" x="12.7" y="-20.32"/>
<gate name="RESET" symbol="BOOT_RST" x="12.7" y="-35.56"/>
</gates>
<devices>
<device name="" package="LQFP48_2">
<connects>
<connect gate="AN_POWER" pin="VBAT" pad="1"/>
<connect gate="AN_POWER" pin="VDDA" pad="9"/>
<connect gate="AN_POWER" pin="VSSA" pad="8"/>
<connect gate="PORTA_H" pin="PA10" pad="31"/>
<connect gate="PORTA_H" pin="PA11" pad="32"/>
<connect gate="PORTA_H" pin="PA12" pad="33"/>
<connect gate="PORTA_H" pin="PA13" pad="34"/>
<connect gate="PORTA_H" pin="PA14" pad="37"/>
<connect gate="PORTA_H" pin="PA15" pad="38"/>
<connect gate="PORTA_H" pin="PA8" pad="29"/>
<connect gate="PORTA_H" pin="PA9" pad="30"/>
<connect gate="PORTA_L" pin="PA0" pad="10"/>
<connect gate="PORTA_L" pin="PA1" pad="11"/>
<connect gate="PORTA_L" pin="PA2" pad="12"/>
<connect gate="PORTA_L" pin="PA3" pad="13"/>
<connect gate="PORTA_L" pin="PA4" pad="14"/>
<connect gate="PORTA_L" pin="PA5" pad="15"/>
<connect gate="PORTA_L" pin="PA6" pad="16"/>
<connect gate="PORTA_L" pin="PA7" pad="17"/>
<connect gate="PORTB_H" pin="PB10" pad="21"/>
<connect gate="PORTB_H" pin="PB11" pad="22"/>
<connect gate="PORTB_H" pin="PB12" pad="25"/>
<connect gate="PORTB_H" pin="PB13" pad="26"/>
<connect gate="PORTB_H" pin="PB14" pad="27"/>
<connect gate="PORTB_H" pin="PB15" pad="28"/>
<connect gate="PORTB_H" pin="PB8" pad="45"/>
<connect gate="PORTB_H" pin="PB9" pad="46"/>
<connect gate="PORTB_L" pin="PB0" pad="18"/>
<connect gate="PORTB_L" pin="PB1" pad="19"/>
<connect gate="PORTB_L" pin="PB2" pad="20"/>
<connect gate="PORTB_L" pin="PB3" pad="39"/>
<connect gate="PORTB_L" pin="PB4" pad="40"/>
<connect gate="PORTB_L" pin="PB5" pad="41"/>
<connect gate="PORTB_L" pin="PB6" pad="42"/>
<connect gate="PORTB_L" pin="PB7" pad="43"/>
<connect gate="PORTC" pin="PC13" pad="2"/>
<connect gate="PORTC" pin="PC14" pad="3"/>
<connect gate="PORTC" pin="PC15" pad="4"/>
<connect gate="PORTD/OSC" pin="PD0/OSC&lt;=" pad="5"/>
<connect gate="PORTD/OSC" pin="PD1/OSC=&gt;" pad="6"/>
<connect gate="POWER" pin="VDD_1" pad="24"/>
<connect gate="POWER" pin="VDD_2" pad="36"/>
<connect gate="POWER" pin="VDD_3" pad="48"/>
<connect gate="POWER" pin="VSS_1" pad="23"/>
<connect gate="POWER" pin="VSS_2" pad="35"/>
<connect gate="POWER" pin="VSS_3" pad="47"/>
<connect gate="RESET" pin="BOOT0" pad="44"/>
<connect gate="RESET" pin="NRST" pad="7"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:7143663/1"/>
</package3dinstances>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="IC1" library="stm32" deviceset="STM32F10XCXT6" device="" package3d_urn="urn:adsk.eagle:package:7143663/1" value="STM32F303CB"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="IC1" gate="PORTA_L" x="109.22" y="137.16" smashed="yes">
<attribute name="NAME" x="106.68" y="147.32" size="1.27" layer="95"/>
</instance>
<instance part="IC1" gate="PORTA_H" x="109.22" y="106.68" smashed="yes">
<attribute name="NAME" x="106.68" y="119.38" size="1.27" layer="95"/>
</instance>
<instance part="IC1" gate="PORTB_L" x="129.54" y="134.62" smashed="yes">
<attribute name="NAME" x="124.46" y="147.32" size="1.27" layer="95"/>
</instance>
<instance part="IC1" gate="PORTB_H" x="129.54" y="109.22" smashed="yes">
<attribute name="NAME" x="124.46" y="119.38" size="1.27" layer="95"/>
</instance>
<instance part="IC1" gate="POWER" x="111.76" y="73.66" smashed="yes">
<attribute name="NAME" x="106.68" y="91.44" size="1.27" layer="95"/>
</instance>
<instance part="IC1" gate="PORTC" x="109.22" y="45.72" smashed="yes">
<attribute name="NAME" x="106.68" y="50.8" size="1.27" layer="95"/>
</instance>
<instance part="IC1" gate="PORTD/OSC" x="132.08" y="88.9" smashed="yes">
<attribute name="NAME" x="124.46" y="91.44" size="1.27" layer="95"/>
</instance>
<instance part="IC1" gate="AN_POWER" x="129.54" y="63.5" smashed="yes">
<attribute name="NAME" x="124.46" y="71.12" size="1.27" layer="95"/>
</instance>
<instance part="IC1" gate="RESET" x="132.08" y="45.72" smashed="yes">
<attribute name="VALUE" x="127" y="38.1" size="1.27" layer="95"/>
<attribute name="NAME" x="127" y="50.8" size="1.27" layer="95"/>
</instance>
</instances>
<busses>
</busses>
<nets>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports the association of 3D packages
with devices in libraries, schematics, and board files. Those 3D
packages will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>
