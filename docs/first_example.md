
The best way to OwnTech is to use our examples. In this section we will upload two examples together.

## Requirements

Before we start, make sure that you meet the requirements below

!!! note "Requirements"
     - Have your [VSCode Environment](vscode_setup.md) already setup
     - Run your first compilation [successfully](vscode_setup.md#step-7---build-the-code).
     - Have an USB-C cable ready
     - Have a SPIN board ready (stand-alone or embedded)

## Setup your hardware

Connect the SPIN board to your computer via the USB.

Notice that the LED2 must turn on.

Here below you see the connection of a SPIN board embedded onto a TWIST board.

![USB Connection of a SPIN board embeeded onto a TWIST board](images/example-fig1-usb_connection.svg)


## Uploading your first code - Blinky


The code you compiled in the [VSCode setup page](vscode_setup.md) is actually a Blinky example.

We will upload this code to the SPIN board. To do so press the flash icon. (1)
{.annotate}

1. The flash icon looks like this: ![flash icon](images/icon-flash.png)

VSCode and PlatformIO will automatically download all the necessary libraries to flash the SPIN board.

If everything goes well, you will get a success as in the image below.

![Success of your first upload](images/example-fig2-upload.png)


!!! tip "Coffee time"
    Depending on your machine, the first flash might take a while.
    Time to pour another coffee.

??? bug "macOS error - mcumgr"

    === "macOS"
         The upload requires a software called **mcumgr**.

         As of the time of writing, we have experienced issues with some macOS devices in finding it.

         Please refer to the [ongoing issue](https://github.com/owntech-foundation/Core/issues/5) to handle it if you have the error below.

         ![macOS error](images/example-fig3-macos_mcuboot_error.png)

You will see the LED blink.

:party_popper: Congratulations! You have uploaded your first code!

## Your first example

Now that you can upload code to your SPIN board, we will use the example wizard of OwnTech.

### Step 1 - Open project tasks

Click on the PlatformIO alien icon. (1)
{.annotate}

1. The alien icon looks like this ![Alien icon](images/icon-platformio.png)




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
