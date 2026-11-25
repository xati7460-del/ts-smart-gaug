TS SMART GAUGE RACING — ANDROID BUILD PACKAGE

Target: ESP32-2432S028 CYD, ESP32-WROOM-32, ST7789 display.
This package contains source code and a GitHub Actions workflow. It is NOT a precompiled firmware.

FILES
- platformio.ini
- TFT_eSPI_User_Setup.h
- src/main.cpp
- .github/workflows/build.yml

ANDROID / GITHUB BUILD
1. Extract this ZIP on your Android phone.
2. In your GitHub repository, upload the extracted project contents (not the ZIP as a single file).
3. Keep the folder structure exactly as shown. In particular, build.yml must be at:
   .github/workflows/build.yml
4. Commit the uploaded files to the main branch.
5. Open the repository's Actions tab.
6. Select "Build TS Smart Gauge Firmware" and open the latest run.
7. Wait for the job to finish successfully.
8. Download the artifact named "TS-SmartGauge-firmware". It contains the generated .bin files.

DISPLAY NOTES
- Native panel size is 240x320; the sketch rotates it to landscape 320x240.
- ST7789 setup and pin definitions are in TFT_eSPI_User_Setup.h.
- Color order/inversion can vary by panel batch; verify the display before installation.

DEMO DATA ONLY
The sketch accepts serial demo telemetry at 115200 baud:
S85,R6500,G3,F72,T88,V13.8

Real motorcycle sensor inputs are NOT implemented. Never connect 12V, ignition-coil,
or spark-plug signals directly to ESP32 GPIO. Use properly designed, isolated signal
conditioning hardware before connecting any vehicle signals.

FLASHING
This package does not include a prebuilt firmware.bin. Only flash binaries after a
successful build. For a standard ESP32 Arduino 4MB build, common offsets are:
- bootloader.bin: 0x1000
- partitions.bin: 0x8000
- boot_app0.bin: 0xE000
- firmware.bin: 0x10000
Confirm the generated build output / flash arguments before flashing.
