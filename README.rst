Installing requirements
=======================

* eclipse with CDT. This is not mandatory, but everything has been configured
  for eclipse and using it will make your life easier.
* gcc with msp430 support. 
* the mspdebug for debuging.

On Debian and its derivatives, you can install the previous requirements::

  # apt-get install gcc-msp430 mspdebug eclipse-cdt

You can get these software from their upstream webpage:

* Eclipse CDT : http://www.eclipse.org/cdt/downloads.php
* msp-gcc and mspdebug :
  http://sourceforge.net/apps/mediawiki/mspgcc/index.php?title=MSPGCC_Wiki (you
  can find instructions for building everything from source)

Getting the source
==================

Get the source from git::

   $ git clone git://github.com/lebipbip/le-BipBip.git

Or get a ZIP archive with the latest code directly from the github project page:

  https://github.com/lebipbip/le-BipBip/zipball/master


Configuring Eclipse
===================

You need to import the project in your workspace:

* right click in the project browser and select : ''Import'',
  ''General''>''Existing project into workspace''
* in the ''Select root directory'', select the directory where you have
  extracted the *le bipbip* source code.
* the import wizard should show 1 project named ''solarclimb'': click on
  ''Finish''.

The project as it is provided in the source code does make any asumption on
where the tools are installed. Instead, it relies on the correct configuration
of your system for finding the ''msp430-gcc'' executable (and all its friends)
in your PATH. If it is not the case (for example if you have installed gcc in
''/usr/local/''), you need to either fix your PATH variable::

  $ export PATH=/path/to/msp-gcc/bin:$PATH

Or you can change the settings in the project:

* right click on the project name
* open the properties dialog
* change your settings to fit your configuration in ''C/C++ Build'' > ''Build''.

You may need to change the settings for gcc, ld and gas.


Building the code
=================

Building the code is as simple as clicking on the ''build project'' icon (small
hammer). Be sure to select the ''Debug'' profile, as its currently the only
profile correctly configured.

Loading the code
================

TODO. Should be something like::

  $ mspdebug rf2500 -q "prog solarclimb.elf"
