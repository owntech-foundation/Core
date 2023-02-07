
This is the OwnTech Power API Core repository.

The OwnTech Power API is modular, allowing to automatically select libraries for use with the power converter.
Using the Core repository, you can work with OwnTech Power API Libraries, which are automatically downloaded on demand.

The Power API is designed to be used with VS Code and PlatformIO.
[More information about PlatformIO on VS code](https://platformio.org/install/ide?install=vscode).


# Downloading OwnTech Power API Core

You fisrt need to clone the Core repository using the following command:

`git clone https://gitlab.laas.fr/owntech/power-api/core.git owntech_power_api`

Then, open VS Code and, if not already done, install the PlatformIO plugin.

Finall, open the newly cloned folder `owntech_power_api` using menu `File > Open Folder...`


# Working with OwnTech Power API

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
Power users may want to tweak them too. Is so, please checkout the [Zephyr documentation](https://docs.zephyrproject.org/latest/).

In the `src` folder, the file `main.cpp` is the entry point of the application.
The file `owntech.ini` is used to configure the Power API Libraries you want to use.


# Working with OwnTech Power API Libraries

To enable a Power API Library, edit the `src/owntech.ini` file.
In this file, you'll find various commented libraries references.
Simply uncomment a line to enable the corresponding library.
You can also add any PlatformIO library in this file.
