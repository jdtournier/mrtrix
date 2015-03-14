MRtrix depends on a number of other packages. Most, if not all of these packages should be available on any recent Linux distribution via the appropriate package manager. MRtrix has been written and tested on Ubuntu Linux 7.10 (Gutsy Gibbon) through to 10.04 (Lucid Lynx). On all these distributions, all dependencies are available via the Synaptic Package Manager.


---

# Compile-time dependencies #

Before any of the programs included in the MRtrix packages can be used, they need to be compiled (i.e. translated into machine code suitable for your specific computer platform). This will generate the required executables and dynamic library. The following packages and corresponding development files (i.e. headers) are required for this step:

  * g++ - the GNU C++ compiler (version > 4.0.0 recommended)
    * available from http://gcc.gnu.org/
  * python (version > 2.5 recommended)
    * available from http://www.python.org/
  * glib (version > 2.16)
  * gtk+ (version > 2.12)
    * both available from http://www.gtk.org/
  * glibmm (version > 2.16)
  * gtkmm (version > 2.12)
    * both available from http://www.gtkmm.org/
  * gtkglext (version > 1.0)
    * available from http://projects.gnome.org/gtkglext/
  * GSL (GNU Scientific Library; version > 1.8)
    * available from http://www.gnu.org/software/gsl/
  * a working OpenGL implementation (version > 1.2)
    * for best results on Linux, we recommend the use of an nVidia® graphics card together with the nVidia® proprietory drivers. These drivers may be available via the distribution package manager (this is the case with Ubuntu Linux). If not, they can be downloaded from http://www.nvidia.com/

On Ubuntu, these packages can be installed via the Synaptic Package Manager (or apt-get) by searching for the following packages:

  * g++
  * python2.5
  * libglib2.0-dev
  * libgtk2.0-dev
  * libglibmm-2.4-dev
  * libgtkmm-2.4-dev
  * libgtkglext1-dev
  * libgsl0-dev
  * libgl1-mesa-dev (for a generic OpenGL implementation);
    * The nVidia® driver already includes the required development files
  * libglu1-mesa-dev


---

# Run-time dependencies #

Once all the executables have been compiled (as in the previous steps), they can be used provided the following packages are also installed. Note that these should already be installed on your system, either by default or as a result of having installed the corresponding development packages in the previous compilation step.

  * glib (version > 2.16)
  * gtk+ (version > 2.12)
    * both available from http://www.gtk.org/
  * glibmm (version > 2.16)
  * gtkmm (version > 2.12)
    * both available from http://www.gtkmm.org/
  * gtkglext (version > 1.0)
    * available from http://www.k-3d.org/gtkglext/Main_Page
  * GSL (GNU Scientific Library; version > 1.8)
    * available from http://www.gnu.org/software/gsl/
  * a working OpenGL implementation (version > 1.2)
    * for best results on Linux, we recommend the use of an nVidia® graphics card together with the nVidia® proprietory drivers. These drivers may be available via the distribution package manager (this is the case with Ubuntu Linux). If not, they can be downloaded from http://www.nvidia.com/

On Ubuntu, these packages can be installed via the Synaptic Package Manager (or apt-get) by searching for the following packages:

  * libglib2.0
  * libgtk2.0
  * libglibmm-2.4
  * libgtkmm-2.4
  * libgtkglext1
  * libgsl
  * libgl1-mesa-glx (for a generic OpenGL implementation);
  * nvidia-glx or nvidia-glx-new (if using a nVidia® graphics card)
  * libglu1-mesa


---

# Compilation from source #

Once all dependencies are installed, MRtrix can be compiled by issuing the commands (modify as appropriate):

```
tar xjf mrtrix-0.2.6.tar.bz2
cd mrtrix-0.2.6/
./build
```

If this completes with no errors, all executables should be located in the `bin/` folder, and the `libmrtrix-0_2_6.so` dynamic library should be located in the lib/ folder.

If any errors do occur, you may need to modify the configuration file. The relevant file will be located in the sysconfig/ folder, will have a `*.py` suffix, and will be named according to the target platform. Unfortunately a full discussion of the various options that can be modified is beyond the scope of this document.

## Compilation on 64-bit systems (x86-64) ##

Compilation on 64-bit systems is now identical to that for 32-bit systems. There is no longer any need to edit the sysconf/linux.py file as in previous versions (the architecture is now set to "-march=native").


---

# Installation of executables #

Once all executables and the dynamic library have been successfully created, they can be made available for use. The recommended installation procedure is as follows (note that this can be tailored to your needs). The following commands will need to be issued from the root account. If you do not have root or administrative access to your system, you can simply install the files to a location that you have write permissions for (i.e. anywhere in your home folder), in which case the following commands can be issued from your own account.

**Note:** on Ubuntu and other systems without a root account, simply type sudo as the first word on the command line (see this page for details).

## copy the executables to a suitable location ##

A suitable location on most Unix/Linux systems is `/opt/` or `/usr/local/`, although any other location is equally valid. In this example, the executables will be placed in `/opt/`. If you wish to install MRtrix in a different location, simply replace all occurences of `/opt` with the desired path.

There are several ways to copy the required files. You can either copy the entire installation folder:
```
cp -r . /opt/mrtrix
```

Or just copy the executables and library:
```
mkdir -p /opt/mrtrix/bin /opt/mrtrix/lib
cp -r bin/* /opt/mrtrix/bin
cp lib/libmrtrix-0_2_6.so /opt/mrtrix/lib
```

**Note:** you may need to alter file access permissions to allow non-root users to run the executables. This can be done using chmod:
```
chmod -R a+rX /opt/mrtrix/
```

## set the correct search paths ##

In order to run the commands from the command line, you will need to set two parameters: the command search path, and the library search path. The procedure for this will depend on your system. On certain systems, it may be possible to set these variables for all users (requires root access). Otherwise, it is relatively simple to set up individual user accounts for MRtrix.

### System-wide installation (all users) ###

Each unix variant may use a different procedure to set up the user's environment upon login. On Ubuntu Linux, the file /etc/environment is used to set environment variables. On other systems, the relevant file may be /etc/profile. Simply open the relevant file as root:
```
sudo gedit /etc/environment
```
and modify the `PATH` variable by adding the `/opt/mrtrix/bin` folder. When you are done, the relevant line in the file should look something like this:
```
PATH="/opt/mrtrix/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
```
You also need to add the folder containing the dynamic library to the system search path. This can be done by creating a file called `/etc/ld.so.conf.d/mrtrix.conf`, containing the path to the relevant folder. The following procedure should be sufficient:
```
gedit /etc/ld.so.conf.d/mrtrix.conf
```
add the line:
```
/opt/mrtrix/lib
```
save the file, and issue the following command:
```
ldconfig
```
Once you have logged out and back in again, you should be able to call all MRtrix commands directly. You can check your installation by typing:
```
mrview
```
which should open the MRtrix viewing tool.

### Single user installation ###

A single user account can be set up to use MRtrix by editing one of the startup scripts. Unfortunately, there is no unique solution here, as distributions vary in their conventions. The simplest solution is to modify your shell's 'rc' script. Note that in this case the programs will only run when launched from the shell (i.e. from the command-line), which should not be a problem in most cases. First, find out which shell you are using:
```
echo $SHELL
```
For the Bourne-again shell (`/bin/bash`), edit the `.bashrc` file in your home directory:
```
gedit ~/.bashrc
```
and add the following two lines at the end of the file:
```
export PATH=/opt/mrtrix/bin:$PATH
export LD_LIBRARY_PATH=/opt/mrtrix/lib/
```

For the Berkeley C shell (`/bin/csh` or `/bin/tcsh`), edit the `.cshrc` file in your home directory:
```
gedit ~/.cshrc
```
and add the following two lines at the end of the file:
```
setenv PATH /opt/mrtrix/bin:$PATH
setenv LD_LIBRARY_PATH /opt/mrtrix/lib/
```

You should now be able to run MRtrix executables from the command-line (note that you may need to open a new terminal or even log out before the changes take effect). You can check your installation by typing:
```
mrview
```
which should open the MRtrix viewing tool.


---

# Final configuration #

Once the software is up and running, is it worth taking the time to perform a few simple configurations steps. The following important application behaviours can be modified by setting the relevant variables in the configuration file (described in more detail in the Appendix). Note that if the configuration file does not exist, or if the variables are not set, default values will be used instead.

## Enable multi-threading ##

If MRtrix is installed on a multi-processor or multi-core system, some of the more CPU intensive tasks can take advantage of this by performing the processing in parallel across some or all of the CPU cores. Currently, the CSD computation and tractography programs are both ready for multi-threading, and will therefore take substantially less time if multi-threading is enabled. This is done by adding the following line to your [configuration file](ConfigurationFile.md):
```
NumberOfThreads: 4
```
with the number set appropriately for your system.

## Left-right convention for Analyse images ##

There is some ambiguity in the Analyse format as to whether images are stored in left to right order or in the reverse order. This can lead to a great deal of confusion, and for this reason, it is not recommended to use the Analyse format if it can be avoided. For those cases when you need to use this format, the left-right ordering convention can be set by adding this line to your [configuration file](ConfigurationFile.md):
```
Analyse.LeftToRight: false
```
By default, Analyse images are assumed to be stored left to right.