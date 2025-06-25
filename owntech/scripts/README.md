# OwnTech PlatformIO scripts

This folder contains helper scripts integrated with PlatformIO to automate tasks related to board setup, firmware upload and data visualization.

## Top-level scripts

- `helper_functions.py` – utility helpers used by other scripts to download and unzip files.
- `pre_bootloader_common.py` – ensures the `mcuboot-image` target is built when uploading a firmware.
- `pre_bootloader_serial.py` – handles USB bootloader upload using `mcumgr` and automatically selects the correct serial port.
- `pre_expert_mode.py` – toggles the VSCode expert mode settings.
- `pre_recovery.py` – reinstalls the OwnTech bootloader over USB using DFU.
- `pre_stlink.py` – switches the board console back to the STLink UART interface.
- `pre_usb.py` – switches the board console to the USB CDC interface.
- `pre_target_examples.py` – exposes examples listed in `owntech/lib/<env>/owntech_examples` as PlatformIO tasks.
- `pre_target_gui_config.py` – launches Zephyr's `guiconfig` to tweak RTOS options.
- `pre_target_install_bootloader.py` – installs the OwnTech bootloader via an external STLink.
- `pre_version.py` – writes the board and shield versions into `zephyr/CMakeLists.txt`.

## Subdirectories

### `monitor`

Contains monitor filters for PlatformIO:

- `filter_recorded_datas.py` – captures data from the `ScopeMimicry` library, saves it in `src/Data_records` and plots it automatically.

### `plot`

Standalone plotting helpers:

- `pre_plot_records.py` – registers the *Plot recording* task that opens a previously saved file with Matplotlib.

Each plotting script defines a `plot_df` function that can be customised to change the appearance of generated figures.
