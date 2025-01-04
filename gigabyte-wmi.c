#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/dmi.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Slava Andrejev");
MODULE_DESCRIPTION("Gigabyte WMI Support");

#define GB_GET_GUID "ABBC0F6F-8EA1-11d1-00A0-C90629100000"
#define GB_SET_GUID "ABBC0F75-8EA1-11d1-00A0-C90629100000"

MODULE_ALIAS("wmi:" GB_GET_GUID);
MODULE_ALIAS("wmi:" GB_SET_GUID);

static struct platform_device *gb_wmi_platform_dev;

// clang-format off
enum gigabyte_wmi_method_id {
	// Fan control methods
	GB_METHOD_CPU_FAN_DUTY      = 70,  // Get CPU Fan Duty (Get & Set)
	GB_METHOD_GPU_FAN_DUTY      = 71,  // GPU Fan Duty (Get & Set)
	GB_METHOD_DEEP_FAN          = 96,  // Deep Fan control (Get & Set)
	GB_METHOD_FAN_HEALTH        = 98,  // Fan Health Status (Get only)
	GB_METHOD_FAN_STEP          = 102, // Set Current Fan Step (Set only)
	GB_METHOD_STEP_FAN_STATUS   = 103, // 10 Steps Fan Status (Get & Set)
	GB_METHOD_FAN_INDEX_VALUE   = 104, // Fan Index Value (Get & Set)
	GB_METHOD_FIXED_FAN_STATUS  = 106, // Fixed Fan Status (Get & Set)
	GB_METHOD_FIXED_FAN_SPEED   = 107, // Fixed Fan Speed (Get & Set)
	GB_METHOD_FAN_PWM_STATUS    = 111, // Fan PWM Status (Get only)
	GB_METHOD_FAN_ADJUST_STATUS = 112, // Fan Adjust Status (Get & Set)
	GB_METHOD_AUTO_FAN_STATUS   = 113, // Auto Fan Status (Get & Set)
	GB_METHOD_TURN_OFF_FAN      = 117, // Turn off fan (Set only)
	GB_METHOD_FAN_SPEED         = 125, // Fan Speed (Get & Set)

	// Power and battery methods
	GB_METHOD_BATT_COUNT         = 72,  // Battery Cycle Count (Get only, you have to call a "set" method to get the value)
	GB_METHOD_MAX_CHARGE         = 73,  // Max Charge (Get & Set)
	GB_METHOD_BATTERY_HEALTH     = 97,  // Battery Health Status (Get only, you have to call a "set" method to get the value)
	GB_METHOD_CHARGE_POLICY      = 100, // Charge Policy (Get & Set)
	GB_METHOD_CHARGE_STOP        = 101, // Charge Stop (Get & Set)
	GB_METHOD_BATT_CYC1          = 109, // Get Battery Cycles1 (Get only)
	GB_METHOD_BATT_CYC           = 110, // Get Battery Cycles (Get only)
	GB_METHOD_CHARGE_MODE        = 128, // Notify EC Power on time 3 months (Set only)
	GB_METHOD_SMART_CHARGE       = 208, // Smart Charge Status (Get & Set)
	GB_METHOD_CHECK_SMART_CHARGE = 209, // Check Smart Charge Support (Get only)

	// Display and brightness controls
	GB_METHOD_BRIGHTNESS          = 192, // Brightness (Get & Set)
	GB_METHOD_BRIGHTNESS_OFF      = 196, // Brightness Off Status (Get & Set)
	GB_METHOD_DECREASE_BRIGHTNESS = 204, // Decrease Brightness (Set only)
	GB_METHOD_INCREASE_BRIGHTNESS = 205, // Increase Brightness (Set only)

	// GPU and graphics methods
	GB_METHOD_PEG_OR_SG         = 80,  // PEG or SG Status (Get & Set)
	GB_METHOD_NV_POWER_CONFIG   = 81,  // Nvidia Power Config (Get & Set)
	GB_METHOD_NV_D1             = 82,  // Nvidia D1 (Set only)
	GB_METHOD_NV_D2             = 83,  // Nvidia D2 (Set only)
	GB_METHOD_NV_D3             = 84,  // Nvidia D3 (Set only)
	GB_METHOD_NV_D4             = 85,  // Nvidia D4 (Set only)
	GB_METHOD_NV_D5             = 86,  // Nvidia D5 (Set only)
	GB_METHOD_NV_THERMAL_TARGET = 87,  // Nvidia Thermal Target (Get & Set)

	// Thermal and sensor methods
	GB_METHOD_THERMAL_DATA   = 86,  // Get Thermal Sensor Data (Get only)
	GB_METHOD_THERMAL_SENSOR = 249, // Query Thermal Sensor Level (Get only)
	GB_METHOD_CPU_TEMP       = 225, // CPU Temperature (Get only)
	GB_METHOD_GPU_TEMP1      = 226, // GPU Temperature 1 (Get only)
	GB_METHOD_GPU_TEMP2      = 227, // GPU Temperature 2 (Get only)
	GB_METHOD_RPM1           = 228, // RPM 1 (Get only)
	GB_METHOD_RPM2           = 229, // RPM 2 (Get only)

	// Performance and power modes
	GB_METHOD_SUPER_QUIET           = 88,  // Set Super Quiet (Set only)
	GB_METHOD_CHECK_HEAVY_LOADING   = 88,  // Check Heavy Loading (Get only)
	GB_METHOD_IS_POWER_SAVING       = 119, // Notify BIOS power saving mode (Set only)
	GB_METHOD_AI_BOOST_STATUS       = 129, // AI Boost Status (Get & Set)
	GB_METHOD_WHISPER_MODE          = 133, // Whisper Mode (Get & Set)
	GB_METHOD_EC_VALUE_BOOST        = 132, // EC Value Boost Status (Get & Set)
	GB_METHOD_TURBO_MODE            = 206, // Turbo Mode Status (Get & Set)
	GB_METHOD_CHECK_SMART_TURBO     = 210, // Check Smart Turbo Support (Get only)
	GB_METHOD_GET_SMART_TURBO_LEVEL = 219, // Smart Turbo Level (Get only)
	GB_METHOD_SET_SMART_TURBO_LEVEL = 220, // Smart Turbo Level (Set only)
	GB_METHOD_DYNAMIC_BOOST         = 231, // Dynamic Boost Status (Get & Set)
	GB_METHOD_SMART_TURBO_STATUS    = 242, // Smart Turbo Enable/Disable (Get & Set)

	// LED and lighting control
	GB_METHOD_LIGHT_BAR          = 89,  // Light Bar Status (Get & Set)
	GB_METHOD_BLUETOOTH_LED      = 115, // Set Bluetooth LED (Set only)
	GB_METHOD_WIFI_LED           = 116, // Set WiFi LED (Set only)
	GB_METHOD_RGB_LED            = 131, // RGB LED Control (Set only)
	GB_METHOD_KEYBOARD_BACKLIGHT = 246, // Keyboard Backlight Level (Get & Set)

	// Device control and status methods
	GB_METHOD_DISABLE_BT_FN_KEY      = 120, // Disable Bluetooth Function Keys (Set only)
	GB_METHOD_DISABLE_COMM_FN_KEY    = 121, // Disable Comm Function Keys (Set only)
	GB_METHOD_CAMERA2                = 126, // Camera2 Status (Get & Set)
	GB_METHOD_TOUCHSCREEN_SUPPORT    = 127, // Touch Screen Support (Get only)
	GB_METHOD_BLUETOOTH              = 193, // Bluetooth Status (Get & Set)
	GB_METHOD_WIFI                   = 194, // WiFi Status (Get & Set)
	GB_METHOD_W35G                   = 195, // W35G Status (Get & Set)
	GB_METHOD_CAMERA                 = 197, // Camera Status (Get & Set)
	GB_METHOD_MUTE_STATUS            = 199, // Set Mute Status (Set only)
	GB_METHOD_TOUCHPAD               = 202, // TouchPad Status (Get & Set)
	GB_METHOD_WINKEY_BLOCKING        = 203, // Winkey Blocking Status (Get & Set)
	GB_METHOD_GET_USB30_STATUS       = 211, // USB 3.0 Status (Get only)
	GB_METHOD_SET_USB30_STATUS       = 212, // USB 3.0 Status (Set only)
	GB_METHOD_CHECK_USB30            = 213, // Check USB 3.0 Support (Get only)
	GB_METHOD_DOCKING_STATUS         = 214, // Docking Status (Get only)
	GB_METHOD_GET_TOUCHSCREEN_STATUS = 215, // TouchScreen Status (Get only)
	GB_METHOD_SET_TOUCHSCREEN_STATUS = 216, // TouchScreen Status (Set only)
	GB_METHOD_LID1_STATUS            = 239, // Lid Status (Get only)
	GB_METHOD_SET_KEYBOARD_MATRIX    = 240, // KeyboardMatrix Status (Set only)
	GB_METHOD_GET_KEYBOARD_MATRIX    = 241, // KeyboardMatrix Status (Get only)
	GB_METHOD_GSENSOR_STATUS         = 243, // G-Sensor Enable/Disable (Get & Set)
	GB_METHOD_ONBOARD_LAN            = 244, // Onboard LAN Enable/Disable (Get & Set)
	GB_METHOD_CHECK_DOCKING          = 245, // Check Docking Support (Get only)

	// System information and time
	GB_METHOD_POWER_ON_TIME     = 99,  // Power On Time (Get & Set)
	GB_METHOD_DEVICE_EXIST      = 108, // Are Devices Exist (Get only)
	GB_METHOD_CHECK_UCF_SUPPORT = 124, // Check UCF Support (Get only)
	GB_METHOD_FIRST_DATE        = 130, // First Date (Get & Set)

	// VR and special modes
	GB_METHOD_VR_STATUS     = 105, // VR Mode Status (Get & Set)
	GB_METHOD_NOTIFY_HDMI   = 245, // Notify BIOS HDMI Connect Status (Set only)
	GB_METHOD_ROTATION_LOCK = 253, // Set Rotation Lock State (Set only)

	// USB charging control
	GB_METHOD_SLEEP_USB_CHARGE       = 122, // USB Charge under Sleep (Get & Set)
	GB_METHOD_HIBERNATION_USB_CHARGE = 123, // USB Charge under Hibernation (Get & Set)

	// Light sensor methods
	GB_METHOD_LIGHT_SENSOR         = 247, // Query Light Sensor Level (Get only)
	GB_METHOD_LIGHT_SENSOR_VERSION = 251, // Light Sensor Version (Get only)
	GB_METHOD_LIGHT_SENSOR_VALUE   = 252, // Light Sensor Value (Get only)

	// 3G/Mobile related
	GB_METHOD_CHECK_3G_MODULE = 254, // Check 3G Module (Get only)
	GB_METHOD_NOTIFY_EC_3G    = 254, // Notify EC 3G Device (Set only)

	// PD control
	GB_METHOD_PD_WARM_RESET = 134, // Set PD Warm Reset (Set only)

	GB_METHOD_LAST = GB_METHOD_NOTIFY_EC_3G + 1
}; // clang-format on

struct gb_get_method_out_descr {
	u8 count;
	u8 size;
};

#define GB_GET_METHOD_OUT_SIZE(name, cnt, type) \
	[name] = { .count = cnt, .size = sizeof(type) }

static const struct gb_get_method_out_descr
	gb_get_method_out_size[GB_METHOD_LAST] = {
		// Fan control methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CPU_FAN_DUTY, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_GPU_FAN_DUTY, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_DEEP_FAN, 10,
				       u8), // 5 temperatures + 5 speeds
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_FAN_HEALTH, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_STEP_FAN_STATUS, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_FAN_INDEX_VALUE, 2,
				       u8), // temperature, speed
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_FIXED_FAN_STATUS, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_FIXED_FAN_SPEED, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_FAN_PWM_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_FAN_ADJUST_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_AUTO_FAN_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_FAN_SPEED, 1, u8),

		// Battery and power methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_BATT_COUNT, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_MAX_CHARGE, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_BATTERY_HEALTH, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHARGE_POLICY, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHARGE_STOP, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_BATT_CYC1, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_BATT_CYC, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_SMART_CHARGE, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHECK_SMART_CHARGE, 1, u8),

		// Display methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_BRIGHTNESS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_BRIGHTNESS_OFF, 1, u8),

		// GPU and graphics methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_PEG_OR_SG, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_NV_POWER_CONFIG, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_NV_THERMAL_TARGET, 1, u8),

		// Thermal and sensor methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_THERMAL_DATA, 3,
				       u8), // Thermal1,2,3
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_THERMAL_SENSOR, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CPU_TEMP, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_GPU_TEMP1, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_GPU_TEMP2, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_RPM1, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_RPM2, 1, u16),

		// Performance and power modes
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHECK_HEAVY_LOADING, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_AI_BOOST_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_WHISPER_MODE, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_EC_VALUE_BOOST, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_TURBO_MODE, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHECK_SMART_TURBO, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_GET_SMART_TURBO_LEVEL, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_DYNAMIC_BOOST, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_SMART_TURBO_STATUS, 1, u8),

		// LED and lighting methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_LIGHT_BAR, 5,
				       u8), // status, level, red, green, blue
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_KEYBOARD_BACKLIGHT, 1, u8),

		// Device control and status methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CAMERA2, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_TOUCHSCREEN_SUPPORT, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_BLUETOOTH, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_WIFI, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_W35G, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CAMERA, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_TOUCHPAD, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_WINKEY_BLOCKING, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_GET_USB30_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHECK_USB30, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_DOCKING_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_GET_TOUCHSCREEN_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_LID1_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_GET_KEYBOARD_MATRIX, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_GSENSOR_STATUS, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_ONBOARD_LAN, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHECK_DOCKING, 1, u8),

		// System information and time methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_POWER_ON_TIME, 5,
				       u8), // year, month, day, hour, minute
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_DEVICE_EXIST, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHECK_UCF_SUPPORT, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_FIRST_DATE, 5,
				       u8), // year, month, day, hour, minute

		// VR and special modes
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_VR_STATUS, 1, u16),

		// USB charging methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_SLEEP_USB_CHARGE, 1, u8),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_HIBERNATION_USB_CHARGE, 1, u8),

		// Light sensor methods
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_LIGHT_SENSOR, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_LIGHT_SENSOR_VERSION, 1, u16),
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_LIGHT_SENSOR_VALUE, 4,
				       u8), // uint8 Byte0,1,2,3

		// 3G/Mobile related
		GB_GET_METHOD_OUT_SIZE(GB_METHOD_CHECK_3G_MODULE, 1, u16),
	};

static int dmi_check_cb(const struct dmi_system_id *dmi)
{
	pr_info("Computer model: '%s'\n", dmi->ident);

	return 1;
}

static const struct dmi_system_id gigabyte_supported_platforms[] __initconst = {
	{ .ident = "Aero 16 YE5",
	  .matches = { DMI_MATCH(DMI_SYS_VENDOR, "GIGABYTE"),
		       DMI_MATCH(DMI_PRODUCT_NAME, "AERO 16 YE5"),
		       DMI_MATCH(DMI_PRODUCT_SKU, "P86YE5") },
	  .callback = dmi_check_cb }
};

struct gigabyte_wmi {
	struct device *dev;
	struct mutex get_lock;
	struct mutex set_lock;
};

static int gigabyte_wmi_set(u32 method_id, void *in_buf, size_t in_size,
			    u32 *out)
{
	struct acpi_buffer input = { in_size, in_buf };
	struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };

	acpi_status status =
		wmi_evaluate_method(GB_SET_GUID, 0, method_id, &input, &output);

	union acpi_object *obj = output.pointer;
	if (out && obj && ACPI_TYPE_INTEGER == obj->type) {
		*out = obj->integer.value;
	}

	if (obj) {
		kfree(obj);
	}

	return ACPI_FAILURE(status) ? -EIO : 0;
}

static int gigabyte_wmi_get(u32 method_id, u8 *in_buf, size_t in_size,
			    void *out_buf, size_t out_size)
{
	if (!out_buf || !out_size) {
		return -EINVAL;
	}
	const u8 el_count = gb_get_method_out_size[method_id].count;
	const u8 el_size = gb_get_method_out_size[method_id].size;
	if (el_count * el_size > out_size) {
		pr_debug("Output buffer is too small\n");
		return -EINVAL;
	}

	struct acpi_buffer input = { in_size, in_buf };
	struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
	acpi_status status =
		wmi_evaluate_method(GB_GET_GUID, 0, method_id, &input, &output);
	if (ACPI_FAILURE(status)) {
		return -EIO;
	}

	union acpi_object *obj = output.pointer;
	if (!obj) {
		goto call_err;
	}

	pr_debug("WMI method %d returned object of type %d\n", method_id,
		 obj->type);

	if (ACPI_TYPE_INTEGER != obj->type) {
		pr_debug("Unexpected return type: %d\n", obj->type);
		if (ACPI_TYPE_BUFFER == obj->type) {
			pr_debug("WMI method %d returned a buffer of size %d\n",
				 method_id, obj->buffer.length);
			if (0 == obj->buffer.length) {
				pr_debug(
					"WMI method %d is probably not implemented in ACPI\n",
					method_id);
			}
		}
		goto call_err;
	}

	switch (el_size) {
	case 1:
		*((u8 *)out_buf) = (u8)(obj->integer.value & U64_C(0xFF));
		out_buf += 1;
		break;
	case 2:
		*((u16 *)out_buf) = (u16)(obj->integer.value & U64_C(0xFFFF));
		out_buf += 2;
		break;
	case 4:
		*((u32 *)out_buf) =
			(u32)(obj->integer.value & U64_C(0xFFFFFFFF));
		out_buf += 4;
		break;
	case 8:
		*((u64 *)out_buf) = obj->integer.value;
		out_buf += 4;
		break;
	default:
		pr_debug("Unexpected element size\n");
		goto call_err;
		break;
	}

	kfree(output.pointer);
	return 0;

call_err:
	kfree(output.pointer);
	return -EIO;
}

static ssize_t cpu_fan_duty_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u8 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_CPU_FAN_DUTY, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetCPUFanDuty(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t cpu_fan_duty_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long cpu_fan_duty;
	int status = kstrtoul(buf, 10, &cpu_fan_duty);
	if (status) {
		return status;
	}

	u32 in_buf = cpu_fan_duty;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_CPU_FAN_DUTY, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetCPUFanDuty(%lu)\n", cpu_fan_duty);

	return count;
}

static ssize_t gpu_fan_duty_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u8 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_GPU_FAN_DUTY, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetGPUFanDuty(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t gpu_fan_duty_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long gpu_fan_duty;
	int status = kstrtoul(buf, 10, &gpu_fan_duty);
	if (status) {
		return status;
	}

	u32 in_buf = gpu_fan_duty;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_GPU_FAN_DUTY, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetGPUFanDuty(%lu)\n", gpu_fan_duty);

	return count;
}

static ssize_t current_fan_step_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long step;
	int status = kstrtoul(buf, 10, &step);
	if (status) {
		return status;
	}

	u32 in_buf = step;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_FAN_STEP, &in_buf, sizeof(in_buf),
				  NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetCurrentFanStepData(%lu)\n", step);

	return count;
}

static ssize_t fixed_fan_status_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u16 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_FIXED_FAN_STATUS, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetFixedFanStatus(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t fixed_fan_status_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long fixed_status;
	int status = kstrtoul(buf, 10, &fixed_status);
	if (status) {
		return status;
	}

	u32 in_buf = fixed_status;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_FIXED_FAN_STATUS, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetFixedFanStatus(%lu)\n", fixed_status);

	return count;
}

static ssize_t fixed_fan_speed_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u16 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_FIXED_FAN_SPEED, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetFanFixedSpeed(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t fixed_fan_speed_store(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long fixed_speed;
	int status = kstrtoul(buf, 10, &fixed_speed);
	if (status) {
		return status;
	}

	u32 in_buf = fixed_speed;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_FIXED_FAN_SPEED, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetFanFixedSpeed(%lu)\n", fixed_speed);

	return count;
}

static ssize_t step_fan_status_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u16 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_STEP_FAN_STATUS, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetStepFanStatus(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t step_fan_status_store(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long step_status;
	int status = kstrtoul(buf, 10, &step_status);
	if (status) {
		return status;
	}

	u32 in_buf = step_status;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_STEP_FAN_STATUS, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetStepFanStatus(%lu)\n", step_status);

	return count;
}

static ssize_t auto_fan_status_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u8 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_AUTO_FAN_STATUS, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetAutoFanStatus(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t auto_fan_status_store(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long fan_status;
	int status = kstrtoul(buf, 10, &fan_status);
	if (status) {
		return status;
	}

	u32 in_buf = fan_status;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_AUTO_FAN_STATUS, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetAutoFanStatus(%lu)\n", fan_status);

	return count;
}

static ssize_t battery_cycle_count_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u32 res;
	mutex_lock(&wmi->get_lock);
	// Because of a bug in the ACPI tables, we must call a set method.
	int status = gigabyte_wmi_set(GB_METHOD_BATT_COUNT, NULL, 0, &res);
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("SetBatteryCount(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t battery_health_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u32 res;
	mutex_lock(&wmi->get_lock);
	// Because of a bug in the ACPI tables, we must call a set method.
	int status = gigabyte_wmi_set(GB_METHOD_BATTERY_HEALTH, NULL, 0, &res);
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("SetBatteryHealth(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t dynamic_boost_status_show(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u8 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_DYNAMIC_BOOST, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	// There is a bug in the ACPI tables. The method returns a Boolean value
	// inverted from the one written. Invert back for consistency.
	res = (0 == res);

	if (status) {
		return status;
	}

	pr_info("GetDynamicBoostStatus(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t dynamic_boost_status_store(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long boost_status;
	int status = kstrtoul(buf, 10, &boost_status);
	if (status) {
		return status;
	}

	u32 in_buf = boost_status;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_DYNAMIC_BOOST, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetDynamicBoostStatus(%lu)\n", boost_status);

	return count;
}

static ssize_t whisper_mode_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u8 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_WHISPER_MODE, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetWhisperMode(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t whisper_mode_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long whisper_mode;
	int status = kstrtoul(buf, 10, &whisper_mode);
	if (status) {
		return status;
	}

	u32 in_buf = whisper_mode;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_WHISPER_MODE, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetWhisperMode(%lu)\n", whisper_mode);

	return count;
}

static ssize_t gpu_temp1_show(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u16 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_GPU_TEMP1, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("getGpuTemperature1(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t gpu_temp2_show(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u16 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_GPU_TEMP2, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("getGpuTemperature2(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t nv_power_config_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u8 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_NV_POWER_CONFIG, NULL, 0, &res,
				      sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetNvPowerConfig(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t nv_power_config_store(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long pwr_cfg;
	int status = kstrtoul(buf, 10, &pwr_cfg);
	if (status) {
		return status;
	}

	u32 in_buf = pwr_cfg;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_NV_POWER_CONFIG, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetNvPowerConfig(%lu)\n", pwr_cfg);

	return count;
}

static ssize_t nv_thermal_target_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	u8 res;
	mutex_lock(&wmi->get_lock);
	int status = gigabyte_wmi_get(GB_METHOD_NV_THERMAL_TARGET, NULL, 0,
				      &res, sizeof(res));
	mutex_unlock(&wmi->get_lock);

	if (status) {
		return status;
	}

	pr_info("GetNvThermalTarget(): %d\n", res);

	return sysfs_emit(buf, "%d\n", res);
}

static ssize_t nv_thermal_target_store(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t count)
{
	struct gigabyte_wmi *wmi = dev_get_drvdata(dev);

	unsigned long therm_tgt;
	int status = kstrtoul(buf, 10, &therm_tgt);
	if (status) {
		return status;
	}

	u32 in_buf = therm_tgt;
	mutex_lock(&wmi->set_lock);
	status = gigabyte_wmi_set(GB_METHOD_NV_THERMAL_TARGET, &in_buf,
				  sizeof(in_buf), NULL);
	mutex_unlock(&wmi->set_lock);

	if (status) {
		return status;
	}

	pr_info("SetNvThermalTarget(%lu)\n", therm_tgt);

	return count;
}

static DEVICE_ATTR_RW(cpu_fan_duty);
static DEVICE_ATTR_RW(gpu_fan_duty);
static DEVICE_ATTR_WO(current_fan_step);
static DEVICE_ATTR_RW(fixed_fan_status);
static DEVICE_ATTR_RW(step_fan_status);
static DEVICE_ATTR_RW(fixed_fan_speed);
static DEVICE_ATTR_RW(auto_fan_status);

static struct attribute *fan_control_attrs[] = {
	&dev_attr_cpu_fan_duty.attr,	 &dev_attr_gpu_fan_duty.attr,
	&dev_attr_current_fan_step.attr, &dev_attr_fixed_fan_status.attr,
	&dev_attr_fixed_fan_speed.attr,	 &dev_attr_step_fan_status.attr,
	&dev_attr_auto_fan_status.attr,	 NULL,
};

static const struct attribute_group fan_control_attribute_group = {
	.name = "fan_control",
	.attrs = fan_control_attrs,
};

static DEVICE_ATTR_RO(battery_cycle_count);
static DEVICE_ATTR_RO(battery_health);

static struct attribute *battery_attrs[] = {
	&dev_attr_battery_cycle_count.attr,
	&dev_attr_battery_health.attr,
	NULL,
};

static const struct attribute_group battery_attribute_group = {
	.name = "battery",
	.attrs = battery_attrs,
};

static DEVICE_ATTR_RW(dynamic_boost_status);
static DEVICE_ATTR_RW(whisper_mode);

static struct attribute *performance_attrs[] = {
	&dev_attr_dynamic_boost_status.attr, &dev_attr_whisper_mode.attr, NULL
};

static const struct attribute_group performance_attribute_group = {
	.name = "performance",
	.attrs = performance_attrs,
};

static DEVICE_ATTR_RO(gpu_temp1);
static DEVICE_ATTR_RO(gpu_temp2);

static struct attribute *sensors_attrs[] = { &dev_attr_gpu_temp1.attr,
					     &dev_attr_gpu_temp2.attr, NULL };

static const struct attribute_group sensors_attribute_group = {
	.name = "sensors",
	.attrs = sensors_attrs,
};

static DEVICE_ATTR_RW(nv_power_config);
static DEVICE_ATTR_RW(nv_thermal_target);

static struct attribute *gpu_attrs[] = { &dev_attr_nv_power_config.attr,
					 &dev_attr_nv_thermal_target.attr,
					 NULL };

static const struct attribute_group gpu_attribute_group = {
	.name = "gpu",
	.attrs = gpu_attrs,
};

static int gigabyte_wmi_probe(struct platform_device *pdev)
{
	struct gigabyte_wmi *wmi;

	wmi = devm_kzalloc(&pdev->dev, sizeof(struct gigabyte_wmi), GFP_KERNEL);
	if (!wmi) {
		return -ENOMEM;
	}

	wmi->dev = &pdev->dev;
	platform_set_drvdata(pdev, wmi);

	if (wmi_has_guid(GB_GET_GUID)) {
		mutex_init(&wmi->get_lock);
	}

	if (wmi_has_guid(GB_SET_GUID)) {
		mutex_init(&wmi->set_lock);
	}

	return 0;
}

static void gigabyte_wmi_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &fan_control_attribute_group);
	sysfs_remove_group(&pdev->dev.kobj, &battery_attribute_group);
	sysfs_remove_group(&pdev->dev.kobj, &performance_attribute_group);
	sysfs_remove_group(&pdev->dev.kobj, &sensors_attribute_group);
	sysfs_remove_group(&pdev->dev.kobj, &gpu_attribute_group);
}

static struct platform_driver gigabyte_wmi_driver = {.driver =
                                                         {
                                                             .name = "gigabyte-wmi",
                                                         },
                                                     .probe  = gigabyte_wmi_probe,
                                                     .remove = gigabyte_wmi_remove};

static int __init gigabyte_wmi_init(void)
{
	if (acpi_disabled) {
		return -ENODEV;
	}

	if (0 == dmi_check_system(gigabyte_supported_platforms)) {
		pr_err("This system is not supported by %s driver\n",
		       THIS_MODULE->name);
		return -ENODEV;
	}

	if (!wmi_has_guid(GB_GET_GUID)) {
		pr_err("%s: No get methods found in WMI ACPI\n",
		       THIS_MODULE->name);
		return -ENODEV;
	}

	if (!wmi_has_guid(GB_SET_GUID)) {
		pr_err("%s: No set methods found in WMI ACPI\n",
		       THIS_MODULE->name);
		return -ENODEV;
	}

	int err = platform_driver_register(&gigabyte_wmi_driver);
	if (err) {
		return err;
	}

	gb_wmi_platform_dev = platform_device_register_simple(
		"gigabyte-wmi", PLATFORM_DEVID_NONE, NULL, 0);
	if (IS_ERR(gb_wmi_platform_dev)) {
		err = PTR_ERR(gb_wmi_platform_dev);
		goto pdev_err;
	}

	err = sysfs_create_group(&gb_wmi_platform_dev->dev.kobj,
				 &fan_control_attribute_group);
	if (err)
		goto dev_err;
	err = sysfs_create_group(&gb_wmi_platform_dev->dev.kobj,
				 &battery_attribute_group);
	if (err)
		goto dev_err;
	err = sysfs_create_group(&gb_wmi_platform_dev->dev.kobj,
				 &performance_attribute_group);
	if (err)
		goto dev_err;
	err = sysfs_create_group(&gb_wmi_platform_dev->dev.kobj,
				 &sensors_attribute_group);
	if (err)
		goto dev_err;
	err = sysfs_create_group(&gb_wmi_platform_dev->dev.kobj,
				 &gpu_attribute_group);
	if (err)
		goto dev_err;

	return 0;

dev_err:
	platform_device_unregister(gb_wmi_platform_dev);
pdev_err:
	platform_driver_unregister(&gigabyte_wmi_driver);
	return err;
}

static void __exit gigabyte_wmi_cleanup(void)
{
	if (gb_wmi_platform_dev) {
		platform_device_unregister(gb_wmi_platform_dev);
		platform_driver_unregister(&gigabyte_wmi_driver);
	}
}

module_init(gigabyte_wmi_init);
module_exit(gigabyte_wmi_cleanup);
