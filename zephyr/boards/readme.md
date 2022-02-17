This folder is shared between Zephyr and PlatformIO.
While embedding PlatformIO content in Zephyr directory may be troubling, this choice has been made in order to reduce the number of directories at the root of the project, reducing the complexity of the project for the end-user.

- As per location (within `Zephyr` folder), this is the default board directory for Zephyr OS, which will use its sub-directory `arm/owntech_board`.
- As per `boards_dir` variable defined in `platformio.ini`, this is the board directory for PlatformIO, which will use its file `owntech_board.json`.

Help links:
- [PlatformIO Custom Boards](https://docs.platformio.org/en/latest/platforms/creating_board.html)
- [Zephyr OS Board Porting Guide](https://docs.zephyrproject.org/latest/guides/porting/board_porting.html)
- [Zephyr OS Application Development Custom Board](https://docs.zephyrproject.org/latest/application/index.html#custom-board-definition)
