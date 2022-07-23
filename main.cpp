/*********         *********/
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include "armour.h"
#include "serial_port.h"
#include "common.h"
#include "base.h"

std::chrono::time_point<std::chrono::steady_clock> start_time;
SerialPort port;
Ptz_infor stm;

int main() {
    start_time = chrono::steady_clock::now();
    Armour armour;
    GX_STATUS status = Config();
    if (status != GX_STATUS_SUCCESS) {
        std::cout << "config Camera Faile ..." << std::endl;
        return 0;
    }
    camera_config cam0_info;
    cam0_info.sn_str = cameraSN;
    cam0_info.SN = &cam0_info.sn_str[0];
    MercureDriver *cam0 = new MercureDriver(cam0_info);
    cam0->InitCamera();
    if (cam0->status != GX_STATUS_SUCCESS) {
        std::cout << "Initial Camera Faile ..." << std::endl;
        return 0;
    }
    status = GXRegisterCaptureCallback(cam0->hDevice_, NULL, Frame_0_ProcessRGB);
    status = GXSendCommand(cam0->hDevice_, GX_COMMAND_ACQUISITION_START);
    if (status != GX_STATUS_SUCCESS) {
        std::cout << "Cam0 Start Read Faile ..." << std::endl;
        return 0;
    }
    while (!port.PortInit(0, 115200));
    std::thread serial_receive_thread(port_receive);
    std::thread armour_auto_shoot(&Armour::run,armour);
//    std::thread armour_auto_shoot(aaaa);
    serial_receive_thread.join();
    armour_auto_shoot.join();


    return 0;
}
