

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/info/info.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>


using mavsdk::ConnectionResult;
using mavsdk::Mavsdk;
using mavsdk::Info;
using mavsdk::System;
using mavsdk::Telemetry;

void usage(std::string bin_name) {
  std::cout << "Usage : " << bin_name << " <connection_url>" << std::endl
            << "Connection URL format should be :" << std::endl
            << " For TCP : tcp://[server_host][:server_port]" << std::endl
            << " For UDP : udp://[bind_host][:bind_port]" << std::endl
            << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
            << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}


static std::atomic_bool g_is_shutdown(false);

void signal_handler(int /*sig*/) { g_is_shutdown.store(true); }


int main(int argc, char** argv) {
  signal(SIGINT, signal_handler);

  if (argc != 2) {
    usage(argv[0]);
    return 1;
  }
  const std::string connection_url = argv[1];

  Mavsdk mavsdk;
  mavsdk.set_configuration(Mavsdk::Configuration::CompanionComputer);
  const ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);
  if (connection_result != ConnectionResult::SUCCESS) {
    std::cerr << "Connection failed: " << connection_result_str(connection_result) << std::endl;
    return 1;
  }

  std::cout << "send_autopilot_version: Waiting to discover system..." << std::endl;
  std::atomic<System*> system(nullptr);
  std::atomic<uint64_t> uid1;
  mavsdk.register_on_discover([&system, &mavsdk, &uid1](uint64_t uuid_param) {
    System* nullptr_ob = nullptr;
    uid1.store(uuid_param);
    if (system.compare_exchange_strong(/*expected=*/nullptr_ob, /*desired=*/&mavsdk.system(uid1))) {
      std::cout << "Discovered system with UID1: " << uid1 << ", hex " << uid1
                << std::endl;
    }
  });

  // Try forever to get a connection
  for (int64_t i = 0; !g_is_shutdown.load(); ++i) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (system.load() != nullptr) {
      break;
    }
    if (i % 60 == 0) {
      std::cout << "send_autopilot_version: Still waiting to discover system" << std::endl;
    }
  }

  Info info(*system);

  while (!g_is_shutdown.load()) {

    // Spin loop at 20Hz
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    info.send_autopilot_version();
    // std::cout << "send autopilot version" << std::endl;
  }
  return 0;
}
