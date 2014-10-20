#include <iostream>
#include <memory>
#include <nfc/nfc.h>
#include <stdexcept>
#include <unistd.h>
#include <vector>


class Context {
  nfc_context *context;
public:
  static std::string version() {
    return nfc_version();
  }
  Context() {
    nfc_init(&context);
    if (!context) {
      throw std::runtime_error("unable to initialize libnfc");
    }
  }
  ~Context() {
    nfc_exit(context);
  }
  Context(const Context &) = delete;
  Context &operator=(const Context &) = delete;
  operator nfc_context *() { return context; }
  operator const nfc_context *() const { return context; }
  std::vector<std::string> list_devices() {
    const size_t max_count = 10;
    nfc_connstring devices[max_count];
    size_t count = nfc_list_devices(*this, devices, max_count);
    return std::vector<std::string>(devices, devices + count);
  }
};


class Target {
  bool valid;
  nfc_target target;
public:
  Target(): valid(false) {}
  Target(nfc_target &target_): valid(true), target(target_) {}
  operator nfc_target *() { return valid ? &target : NULL; }
  operator const nfc_target *() const { return valid ? &target : NULL; }
};


class Device {
  nfc_device *device;
public:
  Device(Context &context) {
    device = nfc_open(context, NULL);
    if (!device) {
      throw std::runtime_error("unable to open device");
    }
  }
  ~Device() {
    nfc_close(device);
  }
  Device(const Device &) = delete;
  Device &operator=(const Device &) = delete;
  operator nfc_device *() { return device; }
  operator const nfc_device *() const { return device; }
  std::string name() {
    return nfc_device_get_name(*this);
  }
};


class InitiatorDevice: public Device {
public:
  InitiatorDevice(Context &context): Device(context) {
    if (nfc_initiator_init(*this) < 0) {
      throw std::runtime_error("unable to set initiator mode");
    }
  }
  InitiatorDevice(const InitiatorDevice &) = delete;
  InitiatorDevice &operator=(const InitiatorDevice &) = delete;
  Target select_passive_target() {
    const nfc_modulation modulations[] = {
      {.nmt = NMT_ISO14443A, .nbr = NBR_106},
      {.nmt = NMT_ISO14443B, .nbr = NBR_106},
      {.nmt = NMT_FELICA, .nbr = NBR_212},
      {.nmt = NMT_FELICA, .nbr = NBR_424},
      {.nmt = NMT_JEWEL, .nbr = NBR_106},
    };
    const size_t modulations_count = sizeof(modulations) / sizeof(modulations[0]);
    const uint8_t poll_period = 2;  // polling period (in units of 150 ms)
    const uint8_t poll_count = 2;  // number of polling
    nfc_target target;
    int result = nfc_initiator_poll_target(*this, modulations, modulations_count, poll_count, poll_period, &target);
    if (result < 0) {
      throw std::runtime_error("unable to select passive target");
    }
    else if (!result) {
      // Got no target.
      return Target();
    }
    return Target(target);
  }
  bool is_present(const Target &target) {
    return nfc_initiator_target_is_present(*this, target) == 0;
  }
};


int
main() {
  std::cout << "libnfc version: " << Context::version() << std::endl;

  Context context;
  std::cout << "devices: [";
  for (std::string device: context.list_devices()) {
    std::cout << " " << device;
  }
  std::cout << " ]" << std::endl;

  InitiatorDevice device(context);
  std::cout << "device name: " << device.name() << std::endl;

  while (true) {
    Target target;
    while (!(target = device.select_passive_target())) {
    }
    std::cout << "target: " << bool(target) << std::endl;

    while (device.is_present(target)) {
      std::cout << "." << std::flush;
      usleep(150000);
    }
    std::cout << std::endl;

    std::cout << "removed" << std::endl;
  }
}
