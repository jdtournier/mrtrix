# Introduction #
MRtrix provides a set of tools to perform diffusion-weighted MRI white matter tractography in the presence of crossing fibres, using Constrained Spherical Deconvolution (Tournier et al.. 2004; Tournier et al. 2007), and a probabilisitic streamlines algorithm (e.g. Behrens et al., 2003; Parker et al., 2003). These applications have been written from scratch in C++, using the functionality provided by the [GNU Scientific Library](http://www.gnu.org/software/gsl/), and [gtkmm](http://www.gtkmm.org/). The software is currently capable of handling DICOM and AnalyseAVW image formats, amongst others. The source code is distributed under the GNU General Public License.

# Acknowledging this work #

If you wish to include results generated using the MRtrix package in a publication, please include a line such as the following to acknowlegde this work:

_Fibre-tracking was performed using the MRtrix package (J-D Tournier, Brain Research Institute, Melbourne, Australia, http://www.brain.org.au/software/) (Tournier et al. 2007)_

# Warranty #

The software described in this manual has no warranty, it is provided "as is". It is your responsibility to validate the behavior of the routines and their accuracy using the source code provided, or to purchase support and warranties from commercial redistributors. Consult the GNU General Public License for further details.

# License #

MRtrix is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

MRtrix is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with MRtrix. If not, see <http://www.gnu.org/licenses/>.

# Support #

For support, please subscribe to the [MRtrix mailing list](http://www.nitrc.org/mailman/listinfo/mrtrix-discussion) and post your questions there. You can also [browse through the archives](http://www.nitrc.org/pipermail/mrtrix-discussion/) to see if your question has already been addressed. Notifications of new releases will also be publicised on that list.

You may also find that your questions have already been answered in the FrequentlyAskedQuestions.

# Supported platforms #

MRtrix should compile and run on any Unix-like system, provided the [GNU Scientific Library](http://www.gnu.org/software/gsl/), [gtkmm](http://www.gtkmm.org/)and a working implementation of [OpenGL](http://www.opengl.org/) (version > 1.2) are available on that system.

| Platform | Notes |
|:---------|:------|
| Linux (x86) | should install on any recent distribution. Tested under Ubuntu 7.10 (Gutsy) and 8.04 (Hardy). See UnixInstallation for instructions. |
| Windows XP | Pre-compiled binaries are available. See WindowsInstallation for instructions |
| MacOS X | Known to compile and run. See MacOSXInstallation for instructions |


# Conventions used in this manual #

This manual contains many examples which can be typed at the keyboard. A command entered at the terminal is shown like this:

**`> command`**

The first character on the line is the terminal prompt, and should not be typed. The greater-than sign '>' is used as the standard prompt in this manual, although some systems may use a different character.

The text that is actually typed at the keyboard is shown in bold. Any output produced by the program will be displayed in a standard font.

In certain cases (e.g. at installation), the number sign '#' is shown as the terminal prompt. In such cases, the command is expected to be executed with administrative (root) privileges.

These examples are for demonstation purposes only: depending on your specific aims and applications, you may want to omit some of the steps shown, or you may need to perform additional steps.

This manual assumes a GNU/Unix environment. There may be minor differences in the input or output on other systems, and you may need to edit the commands to suit your working environment. For example, in Microsoft Windows, you will need to use the backslash '\' as the directory separator.