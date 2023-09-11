# Linux pinctrl-state sysfs Driver

This driver allows control of pinctrl-states at runtime from sysfs,
by creating a special device in device-tree.

## Example

This example uses a pin of i.MX8DXL SoC that supports GPIO as an input only,
to facilitate runtime control of pull-up and pull-down setting:

### Device-Tree declaration

```
/ {
	gps-reset {
		compatible = "pinctrl-state-sysfs";
		pinctrl-0 = <&pinctrl_lpuart2_gps_rst>;
		pinctrl-1 = <&pinctrl_lpuart2_gps_rst_active>;
		pinctrl-names = "default", "active";
	};
};

&iomuxc {
	pinctrl_lpuart2_gps_rst: lpuart2gpsrstgrp {
		fsl,pins = <
			IMX8DXL_SNVS_TAMPER_OUT4_LSIO_GPIO2_IO08_IN		0x0000021
		>;
	};

	pinctrl_lpuart2_gps_rst_active: lpuart2gpsrstactivegrp {
		fsl,pins = <
			IMX8DXL_SNVS_TAMPER_OUT4_LSIO_GPIO2_IO08_IN		0x0000041
		>;
	};
};
```

### sysfs control

```
modprobe pinctrl-state-sysfs.ko

# echo default > /sys/devices/platform/gps-reset/state
[ 1135.319806] pinctrl-state-sysfs gps-reset: selected pinctrl state "default"

# echo active > /sys/devices/platform/gps-reset/state
[ 1165.370890] pinctrl-state-sysfs gps-reset: selected pinctrl state "active"

# echo default > /sys/devices/platform/gps-reset/state
[ 1234.536995] pinctrl-state-sysfs gps-reset: selected pinctrl state "default"
```

# Compile

```
make -C "/lib/modules/$(uname -r)/build" M="$PWD" modules
# make -C "~/linux-build" CROSS_COMPILE="arm-linux-gnueabihf-" ARCH=arm64 M="$PWD" modules
```
