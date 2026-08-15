# Compatibility strategy

Compatibility is a testable product surface. A runtime is not considered
supported because it launches once; it must pass installation, graphics,
audio, file access, suspend/resume, update, and uninstall checks.

## Test matrix

| Surface | First target | Validation |
| --- | --- | --- |
| Native | amd64 FreeBSD userland | boot, package install, networking, suspend |
| Linux | Ubuntu/Debian-style userland | ELF launch, Vulkan, audio, filesystem |
| Gaming | Steam + Proton | controller, shader cache, fullscreen, saves |
| Windows apps | Wine stable | installer, fonts, clipboard, file dialogs |
| Creative | PipeWire + GPU compute | low-latency audio, capture, render/export |

## GPU support

The initial certification list should be deliberately small:

- AMD graphics with the upstream Mesa Vulkan stack.
- Intel integrated graphics with the upstream Mesa Vulkan stack.
- NVIDIA graphics only after the driver, Vulkan, suspend, and Proton paths are
  repeatable on the selected release.

Every certified GPU profile needs a versioned driver/runtime combination.

## Audio and creative workloads

Creative workloads need more than a fast desktop. Validation should cover:

- Stable low-latency PipeWire sessions.
- MIDI input and output.
- JACK-compatible applications where required.
- Screen capture and color-managed display output.
- GPU compute and hardware encoding/decoding.
- Large project files on ZFS without unexpected power-management stalls.

## Known boundaries

The following are compatibility risks rather than silent fallbacks:

- Windows software that requires a Windows kernel driver.
- Anti-cheat or DRM systems that reject Wine/Proton.
- Applications requiring undocumented GPU driver behavior.
- Linux applications that assume a specific distribution's init system.
- Proprietary creative applications with strict hardware or activation checks.

The launcher should show a clear status for each application: verified,
community-tested, experimental, or unsupported.

## Phase 2 gate status

The first Phase 2 implementation records these gates in
`tests/compatibility/README.md`. Tiny Linux ELF and Windows PE hello-world
fixtures now live under `tests/compatibility/fixtures/` and can be generated
with `make compat-fixtures`, but generated host fixtures are only inputs to the
real NovaOS gates. They remain **not run** until the required FreeBSD/
Linuxulator and Wine/Proton runtimes are attached:

- Static Linux ELF hello world
- Dynamic Linux CLI application
- Linux GUI toolkit smoke test
- Win32 console hello world
- Proton launch with Vulkan

Passing a host Linux command or host Wine command is not evidence that NovaOS
compatibility works.
