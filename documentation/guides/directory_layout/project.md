# Projects/ directory
The project directory was made to hold project folders. The projects directory
includes a few template projects that you can use to make your own projects.

The default starter projects are:

- **hello_world** Simple project that prints hello world continuously.
- **starter** An empty template project for use in creating new projects.

!!! Note
    A project folder need not be in the project directory. You can use it if you
    like but you can move the project folder itself around your computer without
    issue.

## Mandatory files/folders

Each of the following files are needed in your project directory:

* **Makefile** used to find the project's main makefile and execute your make
  target requests. If your project does not have one, simply copy one from
  [`projects/starter/makefile`](https://github.com/kammce/SJSU-Dev2/blob/master/projects/starter/makefile).
* **source/** where all of your source code go. Typically these will hold a
  `main.cpp` file. Somewhere within this folder needs to exist a `int main()`
  function so that the startup code knows where to start the application.

## Optional files/folders

* **project.mk** file is used to configure for the build, test and
  debugging of a specific project. The `projects/starter/project.mk` is the
  template `project.mk` file that includes documentation within it as comments.
  You can read and learn more about how to use this by going here
  [`projects/starter/project.mk`](https://github.com/kammce/SJSU-Dev2/blob/master/projects/starter/project.mk).
* **test/** directory holds test files for the project. Each test `.cpp` file
  needs to have a corresponding `USER_TEST` entry in the `project.mk` file.
* **project_config.hpp** file is used to configure aspects of the applications
  code such as processor speed, log level, and more. See
  [`config.hpp`](https://github.com/kammce/SJSU-Dev2/blob/master/library/config.hpp)
  for the complete list of configuration options. See `hello_world/`'s
  [`project_config.hpp`](https://github.com/kammce/SJSU-Dev2/blob/master/projects/hello_world/project_config.hpp)
  for an example of how to use this.

## Build Folder
The build folder is generated everytime you build something using make. It
contains all of the files generated in the process of creating the firmware
binary that you flash onto your board. The build folder contains a folder for
every make-able target.
