# OwnTech Power API

This is the OwnTech Power API Core repository.

The Power API is currently designed to be used with VS Code and PlatformIO.
[More information about PlatformIO on VS code](https://platformio.org/install/ide?install=vscode).

For information about Power API, check out its [Documentation](https://gitlab.laas.fr/owntech/power-api/core/-/wikis/home).

## Downloading OwnTech Power API Core

You fisrt need to clone the Core repository using the following command:

`git clone https://gitlab.laas.fr/owntech/power-api/core.git owntech_power_api`

Then, open VS Code and, if not already done, install the PlatformIO plugin.

Finall, open the newly cloned folder `owntech_power_api` using menu `File > Open Folder...`


## Working with OwnTech Power API

The hierarchy of the project is as follows:

```
owntech_power_api
└─ owntech
|  └─ boards
|  └─ scripts
└─ src
|  └─ main.cpp
└─ zephyr
|  └─ boards
|  └─ dts
|  └─ modules
└─ LICENSE
└─ platformio.ini
└─ README.md
```

You will want to work in the `src` folder. Other folders and files are used to configure the underlying Zephyr OS and PlatformIO, and are hidden by default.
In the `src` folder, the file `main.cpp` is the entry point of the application.
Moreover, some configuration can be done in the `platformio.ini` file.

The API itself is located in `zephyr/modules`. If you need to tailor it to your needs, please checkout the [Zephyr documentation](https://docs.zephyrproject.org/latest/).
