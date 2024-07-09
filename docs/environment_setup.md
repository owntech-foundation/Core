
If you use or wish to use Visual Studio Code, follow this tutorial to set up your work environment.

## Requirements

Before we start, make sure your machine meets all the requirements below.

!!! tips "Required software"
    === "Windows"
        - **Git:** If you do not have git installed, get it here [git for Windows](https://gitforwindows.org)
        - **Python3:** If you do not have python3 installed, get it here [Python3 Installers](https://www.python.org/downloads/windows)
        - **CMake:** If you do not have CMake installed, get it here [CMake Installer](https://cmake.org/download/)
        - **Internet connection**

    === "macOS"

        - **Git:** If you do not have git installed, get it here [git for macOS](https://git-scm.com/download/mac)
        - **Python3:** If you do not have python3 installed, get it here [Python3 Installers](https://www.python.org/downloads/macos/)
        - **CMake:** If you do not have CMake installed, get it here [CMake Installer](https://cmake.org/download/)
        - **Internet connection**

    === "Linux"

        - **Git:** If you do not have git installed, get it here [git for Linux](https://git-scm.com/download/linux)
        - **Python3:** If you do not have python3 installed, get it here [Python3 Installers](https://docs.python-guide.org/starting/install3/linux/)
            - The [pip](https://pip.pypa.io/en/stable/) package installer is needed. If using the system Python, `pip` may not be installed by default.
              See [Installing pip with Linux Package Managers](https://packaging.python.org/en/latest/guides/installing-using-linux-tools/).
            - Warning: some Linux distributions such as Debian and Ubuntu don't install the [venv](https://docs.python.org/3/library/venv.html) module by default
              even though it is part of the Python Standard Library.
              In particular, if using the system Python (`/usr/bin/python3`) on a Debian or Ubuntu system, make sure that the `python3-venv` package is installed.
        - **CMake:** If you do not have CMake installed, get it here [CMake Installer](https://cmake.org/download/)
        - 64 bit linux installation
        - **Internet connection**




## Setup your work environment

To use OwnTech's system, we will use:

* **Visual Studio Code** - The platform or Integrated Development Environment we will use to write code.
* **PlatformIO** - A Visual Studio Code plugin that is a toolbox for microcontrollers

!!! info "Required Memory Space"
    Make sure that you have more than 2GB on your hard drive so that PlatformIO can download all the required files without issues.

Here is how to setup the work environment:

### Step 1 - Empty folder

Create an empty folder in which you will work throughout the tutorials.

!!! warning
    === "Windows"
        - Make sure that you have **administrator privileges** on the folder where you clone your repository.
        - Check that you are working on a **path that is not linked to OneDrive**
        - Check that you **do not have any spaces in the path** of your project
        - Check that the **length of your project** path is smaller than 256 characters.
        - You should preferably have your project folder as close as possible to the root



### Step 2 - Intall VSCode

[Download ](https://code.visualstudio.com/#alt-downloads) and install Visual Studio Code.

### Step 3 - Install PlatformIO

Launch Visual Studio Code.

On the left side menu, click on the extension icon !(1)
{ .annotate }

1. The icon looks like this: ![extension_icon](images/icon-extension.png)

In the search engine, type "PlatformIO IDE", and click install.
Finally, restart Visual Studio Code when you are prompted to do so.

![PlatformIO installation](images/fig1-platformio_installation.png)


### Step 4 - Open PlatformIO in VSCode

In Visual Studio code, on the left side menu, click on PlatformIO icon ! (1)
{ .annotate }

1. The PlatformIO icon looks like this: ![platformio_icon](images/icon-platformio.png)

!!! tip   "Pro tip"
    if the alien icon does not show up spontaneously, wait for a few more seconds, then press F1 key and type platformio home.

### Step 5 - Clone our Core repository
In PlatformIO, select "Clone Git Project ".

PlatformIO will automatically open a field in which you can copy and paste the path below :

```
https://github.com/owntech-foundation/Core
```

PlatformIO will ask you in which folder to clone the project.

Choose the folder you have created previously. A pop up will appear asking if you trust the authors.

You can trust us. :smile:


![Clone the git repository](images/fig2-clone_git_repository.png)


Make sure you are on the `main` branch of the Git project.

You can see it on the bottom left of the VSCode window as shown in the image to the left.

![View of the main branch](images/fig3-main_branch.png)


### Step 6 - Build our core code

In the bottom menu, click on the Build icon. (1)
{ .annotate }

1. The build icon looks like this: ![build_icon](images/icon-build.png).

This will launch the compilation of the code.

If this is the first time that you compile, Visual Studio Code will download several extensions that are required to write the code onto OwnTech’s microprocessor.

!!! tip "Coffee Time"
    During your first build, PlatformIO will download all the necessary dependencies of our code. This may take several minutes depending on your machine and your internet connection.

    Do not hesitate to go get yourself a coffee.

When the compilation is completed, you should see:

![Compilation Complete](images/fig4-compilation_complete.png)


!!! note
    In the unlikely event that you cannot compile, please refer to the troubleshooting section below.

### Step 7 - Prepare your hardware

Before we start, make sure that you meet the requirements below

!!! note "Hardware Requirements"
     - Have your [VSCode Environment](#setup-your-work-environment) already setup
     - Run your first compilation [successfully](#step-6---build-our-core-code).
     - Have an USB-C cable ready
     - Have a SPIN board ready (stand-alone or embedded on a TWIST)

=== " "
    ![USB Connection of a SPIN board embeeded onto a TWIST board](images/example-fig1-usb_connection.svg){align=left}

    - Connect the SPIN board to your computer via the USB.
    - Notice that the LED PWR must turn on.
    - Here you see the connection of a SPIN board embedded onto a TWIST board.



### Step 8 - Upload our core code in your SPIN board

The code you compiled implements a Blinky example.

We will upload this code to the SPIN board. To do so press the flash icon. (1)
{.annotate}

1. The flash icon looks like this: ![flash icon](images/icon-flash.png)

VSCode and PlatformIO will automatically download all the necessary libraries to flash the SPIN board.

If everything goes well, you will get a success as in the image below.

![Success of your first upload](images/example-fig2-upload.png)


!!! tip "Coffee time 2"
    During your first upload PlatformIO will download the necessary depencies to send data to the SPIN board. Depending on your machine and your internet connection, this might take some time.

    Time to pour another coffee.

??? bug "Known bug - mcumgr"

    === "macOS"
         The upload requires a software called **mcumgr**.

         As of the time of writing, we have experienced issues with some macOS devices in finding it.

         Please refer to the [ongoing issue](https://github.com/owntech-foundation/Core/issues/5) to handle it if you have the error below.

         ![macOS error](images/example-fig3-macos_mcuboot_error.png)

You will see the LED blink.

!!! success " :party_popper: Success"
     Congratulations! You have uploaded your first code!


## Troubleshooting

From our exeprience, there are multiple types of errors that can block your compilation.

Check the list below of possible issues

??? bug "Troubleshooting"

    === "Windows"
        - **Git:** If you do not have git installed, get it here [git for Windows](https://gitforwindows.org)
        - **Python3:** If you do not have python3 installed, get it here [Python3 Installers](https://www.python.org/downloads/windows)
        - **CMake:** If you do not have CMake installed, get it here [CMake Installer](https://cmake.org/download/)
        - Make sure that you have **administrator privileges** on the folder where you clone your repository.
        - Check that you are working on a **path that is not liked to OneDrive**
        - Check that you **do not have any spaces in the path** of your project
        - Check that the **length of your project** path is smaller than 256 characters.
        - You should preferably have your project folder as close as possible to the root
        - Be sure the SPIN board PWR LED lights up correctly when connected to the USB
        - Check your USB-C cable is working and can handle data
        - Check your internet connectin is up and running

    === "macOS"
        - **Git:** If you do not have git installed, get it here [git for macOS](https://git-scm.com/download/mac)
        - **Python3:** If you do not have python3 installed, get it here [Python3 Installers](https://www.python.org/downloads/macos/)
        - **CMake:** If you do not have CMake installed, get it here [CMake Installer](https://cmake.org/download/)
        - Be sure the SPIN board PWR LED lights up correctly when connected to the USB
        - Check your USB-C cable is working and can handle data
        - Check your internet connectin is up and running
        - If you have a problem with `mcumgr`, please refer to the [ongoing issue](https://github.com/owntech-foundation/Core/issues/5) to handle it.


    === "Linux"

         - **Git:** If you do not have git installed, get it here [git for Linux](https://git-scm.com/download/linux)
        - **Python3:** If you do not have python3 installed, get it here [Python3 Installers](https://docs.python-guide.org/starting/install3/linux/)
        - **CMake:** If you do not have CMake installed, get it here [CMake Installer](https://cmake.org/download/)
        - Check that your Linux is 64bits
        - Be sure the SPIN board PWR LED lights up correctly when connected to the USB
        - Check your USB-C cable is working and can handle data
        - Check your internet connectin is up and running




??? success "List of contributors"
    Here is a short list of contributors to this page:

    - 2024.02.24: Ayoub Farah, Luiz Villa
    - 2021.11.04: Loïc Quéval, Romain Delpoux, Adrien Prévost
    - 2021.11.07: Luiz Villa, Antoine Boche
    - 2022.01.24: Luiz Villa, Adrien Prevost, Loïc Quéval
    - 2022.03.13: Luiz Villa
    - 2022.05.06: Luiz Villa
    - 2022.06.23: Loïc Quéval
    - 2022.01.16: Mathilde Longuet and Luiz Villa
    - 2023.07.10: Luiz Villa
    - 2023.09.02: Mathilde Longuet
    - 2023.09.25: Mathilde Longuet
