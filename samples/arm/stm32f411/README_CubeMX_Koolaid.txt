***** How to use Cube MX with the Digital Cave build system *****

1) Be sure you have the arm-none-eabi GCC cross compiler installed on your system
2) Create the project folder.
	a) Include a Makefile with (at least) the following lines:
PROJECT=project_name
include ../../../../build/stm32f411.mk
(change the second line to point to the MCU in question; e.g. stm32f410, stm32f411, etc.)
	b) Symlink 'inc' to the project folder
3) Create a new project in CubeMX.  Edit the values in Project -> Settings:
	a) Project Name must be 'cubemx' for the build scripts to work
	b) Project Location must be the project folder you created in step 2.
	c) Toolchain must be TrueSTUDIO
	d) Under Code Generator tab, select "Add necessary library files as reference..."
	e) Hit OK
4) Set and configure clock speed / peripherals / etc as you see fit.
5) Project -> Generate Code to generate the startup code.

At this point, you should be able to go to the project folder and type 'make', and have everything compile properly.

6) You can now start to add your own code.  Make a 'src' folder (sibling of cubemx / inc / Makefile) and add code as you see fit.  It is recommended that you include some sort of pseudo-main file.
7) Insert a call to your pseudo main file in the generated main.c.  Be sure to put this call within BEGIN / END comment block, after setup.  For instance:
  ...
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  pseudoMain();		//Call DigitalCave main project after startup
  while (1)
  {
  /* USER CODE END WHILE */
  ...
