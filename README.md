# mavsdk_tests


## MAVSDK version
https://github.com/DanielePettenuzzo/MAVSDK/tree/daniele-develop-branch

## Custom MAVSDK features
- send_autopilot_version: saves the autopilot_version from the autopilot and then this can be sent again. This was used to test a QGC bug fix where if we received many AUTOPILOT_VERSION messages we would run out of sockets and after some minutes the application would crash.

## Build
mkdir build
cd build
cmake ..
make

## Run
./build/send_autopilot_version <mavlink connection>

Example: ./build/send_autopilot_version udp://:14530