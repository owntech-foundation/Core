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
└─ src
|  └─ main.cpp
|  └─ owntech.ini
└─ zephyr
|  └─ boards
|  └─ dts
|  └─ modules
|  └─ scripts
|  └─ [...]
└─ LICENSE
└─ README.md
└─ [...]
```

You will want to work in the `src` folder, other folders and files are used to configure the underlying Zephyr OS and PlatformIO.
In the `src` folder, the file `main.cpp` is the entry point of the application.
The file `owntech.ini` is used to configure the Power API Libraries you want to use.

The API itself is located in `zephyr/modules`. If you need to tailor it to your needs, please checkout the [Zephyr documentation](https://docs.zephyrproject.org/latest/).


## Working with OwnTech Power API Libraries

Some aditional libraries are available for download.
To enable a Power API Library, edit the `src/owntech.ini` file.
In this file, you'll find commented libraries references.
Simply uncomment a line to enable the corresponding library.
You can also add any PlatformIO library in this file.
