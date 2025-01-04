# Linux WMI Driver for Gigabyte Laptops

This driver enables control over power management and cooling on Gigabyte laptops through Linux sysfs interface. It implements support for Gigabyte's WMI ACPI methods, allowing users to control performance settings that are normally only accessible through Gigabyte's Windows software.

## Background

Gigabyte laptops running Linux often experience performance limitations due to two factors:

1. **ACPI OS Detection**: The EC (Embedded Controller) enforces performance limits when it detects a non-Windows OS. This can be addressed by adding `acpi_os_name="Windows 2015"` to your kernel parameters. To find the correct Windows version string for your laptop:
    ```shell
   mkdir tmp && cd tmp
   sudo acpidump -b
   for x in *.dat; do iasl $x; done
   find -type f -name '*.dsl' -exec grep Windows {} +
    ```

2. **Lack of Linux Support**: Unlike other major laptop manufacturers, Gigabyte has not contributed Linux-specific code to the kernel. This driver aims to bridge that gap by providing Linux interfaces to control laptop hardware.

## Features

The driver creates control interfaces under `/sys/devices/platform/gigabyte-wmi/` with the following structure:

### Battery Management
 * `battery/battery_cycle_count` (read-only)
 * `battery/battery_health` (read-only)

 ### Fan Control
 * `fan_control/auto_fan_status` (read/write)
 * `fan_control/cpu_fan_duty` (read/write)
 * `fan_control/current_fan_step` (write-only)
 * `fan_control/fixed_fan_speed` (read/write)
 * `fan_control/fixed_fan_status` (read/write)
 * `fan_control/gpu_fan_duty` (read/write)
 * `fan_control/step_fan_status` (read/write)

### GPU Settings
 * `gpu/nv_power_config` (read/write)
 * `gpu/nv_thermal_target` (read/write)

### Performance Modes
 * `performance/dynamic_boost_status` (read/write)
 * `performance/whisper_mode` (read/write)

### Sensors
 * `sensors/gpu_temp1` (read-only)
 * `sensors/gpu_temp2` (read-only)

## Usage Examples

### Gaming Mode
```shell
echo 1 > /sys/devices/platform/gigabyte-wmi/performance/dynamic_boost_status
echo 1 > /sys/devices/platform/gigabyte-wmi/gpu/nv_power_config
echo 0 > /sys/devices/platform/gigabyte-wmi/performance/whisper_mode
echo 0 > /sys/devices/platform/gigabyte-wmi/fan_control/current_fan_step
echo 0 > /sys/devices/platform/gigabyte-wmi/fan_control/fixed_fan_status
echo 0 > /sys/devices/platform/gigabyte-wmi/fan_control/step_fan_status
echo 1 > /sys/devices/platform/gigabyte-wmi/fan_control/auto_fan_status
echo 0 > /sys/devices/platform/gigabyte-wmi/gpu/nv_thermal_target
```

### Meeting Mode
```shell
echo 1 > /sys/devices/platform/gigabyte-wmi/performance/dynamic_boost_status
echo 1 > /sys/devices/platform/gigabyte-wmi/gpu/nv_power_config
echo 0 > /sys/devices/platform/gigabyte-wmi/performance/whisper_mode
echo 0 > /sys/devices/platform/gigabyte-wmi/fan_control/current_fan_step
echo 0 > /sys/devices/platform/gigabyte-wmi/fan_control/fixed_fan_status
echo 0 > /sys/devices/platform/gigabyte-wmi/fan_control/step_fan_status
echo 1 > /sys/devices/platform/gigabyte-wmi/fan_control/auto_fan_status
echo 0 > /sys/devices/platform/gigabyte-wmi/gpu/nv_thermal_target
```

### Turbo Mode
```shell
echo 1 > /sys/devices/platform/gigabyte-wmi/performance/dynamic_boost_status
echo 1 > /sys/devices/platform/gigabyte-wmi/gpu/nv_power_config
echo 0 > /sys/devices/platform/gigabyte-wmi/performance/whisper_mode
echo 0 > /sys/devices/platform/gigabyte-wmi/fan_control/current_fan_step
echo 1 > /sys/devices/platform/gigabyte-wmi/fan_control/fixed_fan_status
echo 1 > /sys/devices/platform/gigabyte-wmi/fan_control/step_fan_status
echo 0 > /sys/devices/platform/gigabyte-wmi/fan_control/auto_fan_status
echo 0 > /sys/devices/platform/gigabyte-wmi/gpu/nv_thermal_target
echo 229 > /sys/devices/platform/gigabyte-wmi/fan_control/fixed_fan_speed
echo 229 > /sys/devices/platform/gigabyte-wmi/fan_control/gpu_fan_duty
```

## Supported Models

Currently tested only on:
 * Gigabyte Aero 16 YE5

The driver likely works on other Gigabyte models with similar WMI interfaces. If you'd like support for your model, please:
 * Open an issue with your laptop's model information
 * Or submit a pull request with the necessary additions

## License

GPL
