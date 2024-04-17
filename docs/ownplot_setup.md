
OwnPlot is the software we have developed to easily visualize data. Here we will show you how to install and use it.


## Requirements

Before you start, make sure you meet the requirements below.

!!! note "Requirements"
     - Have your [VSCode Environment](environment_setup.md#setup-your-work-environment) already setup
     - Have your first [PWM example uploaded](first_example.md).
     - Have an USB-C cable ready
     - Have a SPIN board ready and connected to your computer (stand-alone or embedded on a TWIST)
     - Have an internet connection up and running

<!--
## Installing OwnPlot

Choose your system and follow the installation procedure.

### Step 1 - Download Ownplot

=== "Windows"

    Download the file *OwnPlot.Setup.{{ ownplot.version }}.exe*

=== "macOS"
	Download the file *OwnPlot-{{ ownplot.version }}-universal.dmg*

=== "Linux (Debian based)"

	Download the file *OwnPlot_{{ ownplot.version }}_amd64.deb*

=== "Linux (RedHat based)"
	Download the file *OwnPlot_{{ ownplot.version }}_amd64.rpm*

=== "Linux (AppImage)"

	Download the file *OwnPlot-{{ ownplot.version }}.AppImage*


### Step 2 - Install OwnPlot

=== "Windows"

    - Launch the executable *OwnPlot.Setup.{{ ownplot.version }}.exe*
	- The app will automatically install in the `User\AppData\Local\Programs\OwnPlot` folder

=== "macOS"
	!!! success "Compatibility"
		This works with Intel & Apple silicon

	1. Launch the disk image *OwnPlot-{{ ownplot.version }}-universal.dmg*
	2. Drag and drop OwnPlot into your Applications folder
	![mac install](imgs/OwnPlot_macOS_install.png)

=== "Linux (Debian based)"

	1. Open a terminal
	2. Install the package
	``` shell
	sudo dpkg -i OwnPlot_{{ ownplot.version }}_amd64.deb
	```

=== "Linux (RedHat based)"
	!!! warning
		This install has not been tested yet

	1. Open a terminal
	2. Install the package
	``` shell
	sudo rpm -i OwnPlot_{{ ownplot.version }}_amd64.rpm
	```

=== "Linux (AppImage)"

	Right-click on the AppImage file

    Check the button to allow it to run as a program


### Step 3 - Run OwnPlot


=== "Windows"

	- OwnPlot should launch itself right after the installation has been completed.
	- OwnPlot should be available from the start menu

=== "macOS"

    Launch OwnPlot from your Application folder or the LaunchPad

	!!! note "If you have trouble running the app for the first time"
		1. If you get this:

		![Mac cannot run](imgs/OwnPlot_macOS_cant_run.png){ width="300" }

		2. Open System Preferences
		3. Go to *Security & Privacy* Tab
		4. Allow OwnPlot to run by clicking the button "Allow"
		![mac Allow app](imgs/OwnPlot_macOS_open_anyways.png)

=== "Linux (Debian based)"

    Type `ownplot` on the terminal.

=== "Linux (RedHat based)"

    Type `ownplot` on the terminal.

=== "Linux (AppImage)"

    Double-click on the AppImage file



Great! Now that you have OwnPlot installed and all setup, we can proceed


OwnPlot latest releases can be downloaded on [github](https://github.com/owntech-foundation/OwnPlot/releases)

!!! warning "Performances issues"


	As of today, The last version available is [v0.6.0](https://github.com/owntech-foundation/OwnPlot/releases/tag/release-0.6.0).

	However, **v0.6.0** has been reported to have a low-performance. If you have trouble running this release, fall back on version [v0.5.1](https://github.com/owntech-foundation/OwnPlot/releases/tag/release-0.5.1)
-->

 {% include '/OwnPlot/docs/user-manual/setup.md' %}

