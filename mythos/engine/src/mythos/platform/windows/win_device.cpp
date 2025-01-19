#include <mythos/input/device.hpp>

#ifdef MYL_OS_WINDOWS
#   include <mythos/log.hpp>
#   include <mythos/platform/windows/win_utility.hpp>

#   include <myl/memory.hpp>

#   include <Windows.h>
#   include <hidsdi.h>
#   include <hidpi.h>

namespace mye {
    auto device::create(device* device) -> bool {
        /// MYTODO: The handle carried by device is a raw input handle,
        /// because of this any HIDD_ or HIDP_ function will fail as they require a different handle.
        /// This needs to be resolved as some infomation maybe not be available with the raw input API.
        
        // Device name

        UINT buffer_size{};
        GetRawInputDeviceInfoW(device->handle, RIDI_DEVICENAME, NULL, &buffer_size);

        // Microsoft docs are incorect, pcbSize is number of characters not bytes
        myl::buffer buffer(buffer_size * sizeof(wchar_t));
        if (GetRawInputDeviceInfoW(device->handle, RIDI_DEVICENAME, buffer.data(), &buffer_size) == static_cast<UINT>(-1)) {
            MYTHOS_ERROR("GetRawInputDeviceInfoW failed: {}", win::last_error_as_string());
            return false;
        }

        if (buffer_size == 0)
            MYTHOS_WARN("Device has no name. Handle: {}", reinterpret_cast<std::uintptr_t>(device->handle));
        else {
            const int wstring_size = buffer.bytes() / sizeof(wchar_t);
            const int string_size = WideCharToMultiByte(CP_ACP, 0, buffer.as<wchar_t>(), wstring_size, NULL, 0, NULL, NULL);
            device->info.name.resize(string_size, 0);
            WideCharToMultiByte(CP_ACP, 0, buffer.as<wchar_t>(), wstring_size, &device->info.name[0], string_size, NULL, NULL);
        }

        // Vendor/Product IDs + revision status

        GetRawInputDeviceInfoW(device->handle, RIDI_DEVICEINFO, NULL, &buffer_size);
        buffer.reallocate(buffer_size);
        if (GetRawInputDeviceInfoW(device->handle, RIDI_DEVICEINFO, buffer.data(), &buffer_size) == static_cast<UINT>(-1)) {
            MYTHOS_ERROR("GetRawInputDeviceInfoW failed: {}", win::last_error_as_string());
            return false;
        }

        const RID_DEVICE_INFO rid = *buffer.as<RID_DEVICE_INFO>();
        device->info.vendor_id  = static_cast<myl::u16>(rid.hid.dwVendorId);
        device->info.product_id = static_cast<myl::u16>(rid.hid.dwProductId);
        device->info.revision   = static_cast<myl::u16>(rid.hid.dwVersionNumber);

        // Additional info

        GetRawInputDeviceInfoW(device->handle, RIDI_PREPARSEDDATA, NULL, &buffer_size);
        buffer.reallocate(buffer_size);
        if (GetRawInputDeviceInfoW(device->handle, RIDI_PREPARSEDDATA, buffer.data(), &buffer_size) == static_cast<UINT>(-1)) {
            MYTHOS_ERROR("GetRawInputDeviceInfoW failed: {}", win::last_error_as_string());
            return false;
        }

        const PHIDP_PREPARSED_DATA preparsed_data = reinterpret_cast<PHIDP_PREPARSED_DATA>(buffer.data());
        HIDP_CAPS capabilities{};
        if (HidP_GetCaps(preparsed_data, &capabilities) != HIDP_STATUS_SUCCESS) {
            MYTHOS_ERROR("HidP_GetCaps failed: {}", win::last_error_as_string());
            return false;
        }

        device->info.usage = {
            .page = static_cast<myl::u16>(capabilities.UsagePage),
            .id   = static_cast<myl::u16>(capabilities.Usage),
        };

        device->info.input_report_length   = static_cast<myl::u32>(capabilities.InputReportByteLength);
        device->info.output_report_length  = static_cast<myl::u32>(capabilities.OutputReportByteLength);
        device->info.feature_report_length = static_cast<myl::u32>(capabilities.FeatureReportByteLength);

        return true;
    }

    auto device::destroy(device* device) -> void {

    }
}
#endif
