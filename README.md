# MeetOverlay

**Experimental: not yet validated in a live Google Meet call. Do not rely on it
as a safety indicator.** MeetOverlay is an independent Chromium MV3 extension
and Qt 6 / LayerShellQt Wayland overlay that attempts to show Google Meet's own
microphone state, without controlling the microphone or using PipeWire state.

Red means muted, green means active and grey means unknown. The indicator is
click-through and does not request keyboard focus. The extension observes DOM
changes and is independent of MeetMute. Google does not document a stable DOM
interface; unsupported languages, changed controls, ambiguous controls or more
than one Meet tab produce unknown. A grey indicator is not proof of mute.

## Status

The application, host, extension and packaging are under development. Synthetic
unit and local IPC tests pass. An Arch package was built, installed, reinstalled
and removed locally; no live Meet test, Brave extension installation test,
version-to-version upgrade, AUR publication, multi-monitor or HiDPI validation
has been completed. Drag-to-position is not implemented. Do not
use this project where a wrong indicator could cause harm. See
[architecture](docs/architecture.md) for limitations and lifecycle details.

## Installation (development)

The AUR package has **not been published**, so `yay -S meetoverlay-git` is not
currently available. When it has passed the required checks and is published,
that command is intended to install it.

Build on Arch Linux with `cmake`, `ninja`, `qt6-base`, `qt6-declarative`,
`qt6-wayland`, `layer-shell-qt`, Python and Node.js installed:

```sh
cmake -S . -B build -G Ninja -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
ctest --test-dir build --output-on-failure
node --test tests/*.test.mjs
```

The package installs the unpacked extension to `/usr/share/meetoverlay/extension`.
In Brave open `brave://extensions`, enable Developer mode and explicitly load
that directory as an unpacked extension. Its ID must be
`kmblkihdhjjmpbbcmlagojgpijoebpfc`. Reload existing Meet tabs after installation.
The Native Messaging host manifest is installed system-wide for Chromium and
Chrome; Brave's use of the Chrome fallback still requires verification. Flatpak
and Snap browsers are not supported by this package configuration.

Start `meetoverlay` manually and open `meetoverlay --settings` to configure it.
The package includes an inactive user service: enable it only if desired with
`systemctl --user enable --now meetoverlay.service`. Alternatively copy
`/usr/share/meetoverlay/meetoverlay-autostart.desktop` to
`~/.config/autostart/`; do not enable both. Neither is activated automatically.
Settings are saved in `$XDG_CONFIG_HOME/meetoverlay/settings.ini` (normally
`~/.config/meetoverlay/settings.ini`). Changes are watched by the running
indicator. Removing the package does not remove user settings.

There is no telemetry. See [MIT license](LICENSE).
