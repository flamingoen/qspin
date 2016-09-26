# qspin

## Changelog

### Planed

- Major code clean up
- Change representation of data types (Data type class)
  - Add aditional data types to the simulator
- Fix issue of some parts of simulations running in UI thread

### 23/9-2016
- Added a scroll area around the simulation tabs
- Now showing which process is running each command in the step list
- Status of the runs are now loged in the output log
- Program will no longer crash if their is no trail file
- Fixed issue where program would crash when in promela parsing multiple declerations in one line

### 14/9-2016
- Added 'save as' function
- Added 'new' function
- Program will now show process errors (ex. when SPIN is missing)
- Program will now only clears log when a new process starts
- Fixed tab length in editor
- Added save warning when trying to overwrite unsaved chanegs
- Will now clear und-redo stack of document when loaded

## Installation

QSpin will for all three supported operating systems need a c compiler added to the path and a working version of SPIN. Mac and Linux will need to have SPIN in their path, for windows it is shipped with the zipped release. On windows mingw can be used as a c compiler

### Linux:

Make sure Qt5 is installed on your computer. Follow the guidelines for your distrubution. When Qt5 is installed, do the following:

1: Clone or download project from github

2: From a terminal, go to the cloned qspin-master folder

3: Run 'qmake'

4: Run 'make install' as administrator

An executable should now be in the qspin-master folder. This can be placed wherever you want the program.

### Mac:

Thanks to github user kfl for the these instructions on how to install qt5 on MacOS(10.10).

  For macOS (10.10) I've had success with the following to get a working Qt5 installation from homebrew:

  1: brew install qt5

  2: brew link --force qt5

  But be warned that this will conflict with Qt4 which is the current default in homebrew for some reason.

After Qt5 have been installed, do the following:

1: Clone or download project from github

2: From a terminal, go to the cloned qspin-master folder

3: Run 'qmake'

4: Run 'make install' as administrator

### Windows:

In the realese tab their will be a zip with the executable and SPIN. The spin folder needs to be in the same folder as the QSpin executable in order to run. It is also important that a c compiler is present and able to run from the path with the command 'cc'
