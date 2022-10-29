#define BOOST_USE_WINDOWS_H
#include <boost/asio.hpp>
#include <windows.h>
#include <SetupAPI.h>
#include <iostream>
#include <devguid.h>
#include <vector>
#include <algorithm>
std::vector<std::wstring>FindUsbDevices(GUID*& guid, HDEVINFO &DeviceInfoSet) {

    int memberIndex = 0;
    TCHAR buffer[500];
    std::vector<std::wstring>foundUsbs;
    while (true) {

        SP_DEVINFO_DATA deviceInfoData;
        ZeroMemory(&deviceInfoData, sizeof(SP_DEVINFO_DATA));
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        if (SetupDiEnumDeviceInfo(DeviceInfoSet, memberIndex, &deviceInfoData) == FALSE) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS) {
                break;
            }
        }
        DWORD nSize = 0;
        SetupDiGetDeviceInstanceId(DeviceInfoSet, &deviceInfoData, buffer, 500, &nSize);
        foundUsbs.push_back(std::wstring(buffer));
        memberIndex++;
    }
    return foundUsbs;
}

void shutdown() {
    system("C:\\Windows\\System32\\shutdown /s /t 0 /f");
}

int main() {
    GUID* guid = (GUID*)&GUID_DEVCLASS_USB;
    std::wstring usb = L"USB\\VID_0666&PID_1777\\60A44C413DF8999888777666";
    boost::asio::io_context io;
    std::vector<std::wstring> foundUsbs;
    while (true) {
        boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
        t.wait();
        HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT | DIGCF_PROFILE);
        foundUsbs = FindUsbDevices(guid, DeviceInfoSet);
        if (std::find(foundUsbs.begin(), foundUsbs.end(), usb) == foundUsbs.end())
        {
            shutdown();
            break;
        }
    }
}
  