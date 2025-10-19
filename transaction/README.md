# Homework for OOP course

## Prerequisites

* C++ Compiler (GCC, Clang, or MSVC)
* **Meson** (Version 1.9.x or newer is recommended)
* **Ninja**

---

## How to Build

Once all prerequisites are installed, building the project is straightforward:

### 1. Set Up The Build Directory
This command only needs to be run once. It creates the `build` directory and detects your compiler and dependencies.

```bash
meson setup build
````

### 2\. Compile The Project

To compile (or re-compile after making changes), run:

```bash
ninja -C build
```

The final executable will be located inside the `build` directory.

3. Run executable file
```bash
./build/app 
```

-----

##  Installing Meson

System package managers (like `apt`) often provide old versions of Meson. The methods below describe how to install the default version or upgrade to the latest version from source.

For a full list of all installation methods (e.g., `pip`), see the [official Meson documentation](https://mesonbuild.com/SimpleStart.html).

### On Linux (Ubuntu/WSL)

**Method 1: Using APT (Easy, but may be an old version)**

This command installs all necessary build tools, but the Meson version might be outdated (e.g., 1.3.x).

```bash
# Installs build-essential, Meson, and Ninja from the Ubuntu repositories
sudo apt install build-essential meson ninja-build
```

**Method 2: Upgrading from Source (Git + Symlink)**

This method replaces the old `apt` version with the latest version from GitHub by manually creating a symbolic link.

1.  Clone the Meson repository to your home directory:

    ```bash
    # Go to your home directory
    cd ~
    git clone https://github.com/mesonbuild/meson.git
    ```

2.  Back up the old Meson binary (if it exists):

    ```bash
    # This moves /usr/bin/meson to /usr/bin/meson-old
    sudo mv /usr/bin/meson /usr/bin/meson-old
    ```

3.  Create a symbolic link from the new Meson python script to `/usr/bin/meson`:
    *(This command uses `$USER` which automatically resolves to your username)*

    ```bash
    sudo ln -s /home/$USER/meson/meson.py /usr/bin/meson
    ```

4.  Verify the new version:

    ```bash
    meson --version
    # Output should be the newest one (e.g., 1.9.x)
    ```

### On Windows

**Method 1: Using the MSI Installer**

1.  Download the latest `.msi` installer from the [Meson GitHub Releases page](https://github.com/mesonbuild/meson/releases).
2.  Run the installer. It will automatically install Meson, Ninja, and configure your system `PATH`.
3.  Restart any open terminals to see the changes.

**Method 2: Installing from Source (Git + .bat wrapper)**

This method gives you the latest development version directly from the Git repository.

**Prerequisite:** This method requires you to have **Python** installed on your Windows machine and added to your `PATH`. Download it from [python.org](https://www.python.org/).

**1. Clone the Meson Repository**

Clone it to a permanent location you will not delete, for example, inside your user profile folder:

```bash
git clone https://github.com/mesonbuild/meson.git C:\Users\%USERNAME%\meson
```

**2. Create the `.bat` Wrapper**

Windows can't run `.py` files from the `PATH` directly. We need to create a "bridge" file.

1.  Create a new folder for your custom scripts, for example: `C:\Users\%USERNAME%\scripts`

2.  Inside `C:\Users\%USERNAME%\scripts`, create a new file named `meson.bat`.

3.  Open `meson.bat` in a text editor and paste the following.
    *(This path should now be correct if you followed step 1).*

    ```batch
    @echo off
    python "C:\Users\%USERNAME%\meson\meson.py" %*
    ```

**3. Add Your Scripts Folder to the Environment PATH**

Add your scripts folder (`C:\Users\%USERNAME%\scripts`) to your user's `PATH` Environment Variablem so Windows can find `meson.bat`.

**4. Verify the Installation**
**Close all your terminals** and open a **new** one. The new terminal will load the updated PATH.

```bash
meson --version
# It should output the latest version, e.g., 1.9.x
```