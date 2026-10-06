# M5Stack ATOM Lite USB Serial Access on Windows, WSL 2, and Docker

This note records the current setup and the results observed with the ATOM Lite's USB serial interface. Windows device names, USB bus IDs, and Linux device paths can change; treat the values below as examples unless explicitly marked as observed on this machine.

## Connection path

The USB serial connection passes through several layers:

1. Windows detects the USB serial converter. The converter observed on this machine identifies as FTDI `0403:6001` / `USB Serial Converter`. To use it from a Windows serial application, the FTDI Virtual COM Port (VCP) driver makes it appear as a `COM` port; check **Device Manager → Ports (COM & LPT)**. If it appears only as a converter and no `COM` port is listed, check the FTDI driver status. USB/IP forwarding to WSL uses the USB device and `BUSID`, not the Windows `COM` number.
2. `usbipd-win` shares the USB device from Windows and attaches it to WSL 2. The `BUSID` shown by `usbipd list` is dynamic.
3. Ubuntu-22.04 in WSL exposes the serial interface as a Linux device, observed here as `/dev/ttyUSB0`.
4. The optional Docker Compose overlay exposes the WSL `/dev` tree inside the container at `/host-dev`; the observed container path is `/host-dev/ttyUSB0`.

The `COM` port, USB `BUSID`, and `/dev/ttyUSB*` path belong to different layers. They are not interchangeable.

## Attach the device to WSL

In Windows PowerShell, find the current bus ID:

```powershell
usbipd list
```

Sharing a device requires an elevated PowerShell prompt. Share it once, using the current bus ID:

```powershell
usbipd bind --busid <BUSID>
```

Then, in a normal PowerShell prompt, attach it to WSL:

```powershell
usbipd attach --wsl --busid <BUSID>
```

The `/sys/bus/platform/drivers/vhci_hcd` path indicates that the WSL USB/IP virtual host controller driver is registered. It is expected to remain present when the ATOM Lite is unplugged; it does **not** indicate whether the USB device is currently attached. In the latest successful attach on this machine, `usbipd` reported `Loading vhci_hcd module` itself. Check the serial device node and `usbipd list` to determine device state.

If `usbipd attach` reports that the WSL kernel is not USB/IP capable, check whether WSL commands still work and whether the controller path exists:

```powershell
wsl -d Ubuntu-22.04 -u root -- modprobe vhci-hcd
wsl -d Ubuntu-22.04 -u root -- ls -ld /sys/bus/platform/drivers/vhci_hcd
```

The kernel version observed was `6.18.40.1`; Microsoft documents `5.10.60.1` or later as the USB/IP kernel prerequisite. A sufficiently new version alone did not guarantee a successful attach in the earlier attempt.

Check the Linux device from PowerShell:

```powershell
wsl -d Ubuntu-22.04 -u root -- ls -l /dev/ttyUSB0
```

The device may instead appear as another `/dev/ttyUSB*` or `/dev/ttyACM*` path depending on the hardware and enumeration order.

## Run the container without a device

The base `docker-compose.yml` does not require a serial device. In Ubuntu, from the repository root:

```bash
./setup.sh
docker compose up --build -d
```

`setup.sh` writes the host `dialout` group ID to `.env` even when no serial device is attached. The base service can therefore start without the ATOM Lite.

## Optional serial-device overlay

To run the service with serial-device access, include the overlay:

```bash
docker compose \
  -f docker-compose.yml \
  -f docker-compose.serial.yml \
  up --build -d
```

The overlay currently mounts host `/dev` at `/host-dev`, adds the host `dialout` group ID, and permits character devices with major number `188` through the container device cgroup. The serial device path inside the container is therefore `/host-dev/ttyUSB0`, not `/dev/ttyUSB0`.

This configuration is intentionally optional. Mounting all of `/dev` and allowing `c 188:* rw` grants broader device access than mapping one fixed serial device. Use the overlay only when that access is acceptable for this development container.

Check visibility inside the running service with:

```bash
docker compose \
  -f docker-compose.yml \
  -f docker-compose.serial.yml \
  exec python-dev ls -l /host-dev/ttyUSB0
```

## Recommended workflow for the current setup

- For normal development without the ATOM Lite, use only `docker compose up --build -d` with the base Compose file.
- When the device is plugged in, use `usbipd list` and attach the current `BUSID` to WSL as described above.
- Attaching the USB device to WSL does not by itself expose it to a container started with only the base Compose file. If the Python container needs serial access, start it with the optional serial overlay. The overlay can start while the device is absent and exposes a later device node through `/host-dev`.
- After physically unplugging, run `usbipd list` and attach the current bus ID again. A later reattach succeeded on this machine, but the container was stopped at that point; live-container reconnect behavior remains unverified.

## Hot-plug test results on this machine

The optional overlay was tried to avoid recreating the container for every physical plug or unplug:

- The container started while the serial device was absent.
- After a successful USB/IP attach, the host device appeared at `/dev/ttyUSB0`; a container started with the overlay could see `/host-dev/ttyUSB0`.
- When the device was unplugged, `/host-dev/ttyUSB0` disappeared inside the container. The container remained running, and its container ID and start time did not change during that unplug check.
- In one reconnect attempt, `usbipd attach` reported `WSL kernel is not USBIP capable`; `modprobe vhci-hcd` then returned exit code 1, and WSL command execution and PTY allocation became unhealthy until `wsl --shutdown` was run. After that shutdown, `usbipd attach` succeeded and reported `Loading vhci_hcd module`; `/dev/ttyUSB0` appeared in Ubuntu. Repeated checks showed `/sys/bus/platform/drivers/vhci_hcd` both with the USB device attached and after unplug/replug; this is expected because the path represents the controller driver, not the serial device.
- After the successful reattach, the serial-overlay container was started and could see `/host-dev/ttyUSB0`. `sudo` in Ubuntu then began failing with `unable to allocate pty`. Stopping the container did not clear the error. The timing is recorded, but a causal relationship between the container and the WSL PTY failure has not been established. A WSL restart had recovered an earlier occurrence; the latest occurrence was left for later troubleshooting.

Therefore, unplugging did not require a container restart in the observed check, and a later WSL reattach succeeded, but reliable unplug-and-reconnect access from a continuously running container has **not** been verified. A persistent `vhci_hcd` sysfs path is normal and does not establish whether the serial device reattached. The precise cause of the WSL PTY failure remains undetermined. `wsl --shutdown` stops all WSL distributions, including Rancher Desktop, and recovered an earlier occurrence in this test.

## References

- [Microsoft: Connect USB devices to WSL](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)
- [usbipd-win: WSL support](https://github.com/dorssel/usbipd-win/wiki/WSL-support)
- [FTDI: Virtual COM Port drivers](https://ftdichip.com/drivers/vcp-drivers/)
