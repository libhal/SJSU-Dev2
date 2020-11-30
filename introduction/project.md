# Creating a Project

The default starter projects that can be found in the `projects/` directory are:

- `project/hello_world`: Simple project that prints hello world continuously.
- `project/starter`: An empty template project for use in creating new projects.
- `project/barebones`: An empty project with ALMOST ALL features turned off and
  highest size optimizations turn on. Used to see the size of the barest version
  of an application is.

## Creating a new project

To create a project, simply copy the `starter` project folder, paste it
somewhere on your computer, and rename it. The folder can be placed anywhere on
the computer. Now you can edit the `source/main.cpp`, add files, edit the
configuration files, etc.

## Details About Projects

### Mandatory project files and folders

Each of the following files are needed in your project directory:

- `Makefile`: required for running `make`.
- `source/main.cpp`: required for building executables.
    - Must have a definition of `int main()` or the build will fail.

### Optional files/folders

- `project.mk`: Used to configure the build of a project. Read the comments of
  the source file to get a better understanding of what can be changed:
  [`projects/starter/project.mk`](https://github.com/SJSU-Dev2/SJSU-Dev2/blob/master/projects/starter/project.mk).
- `test/`: A place to hold test files.
- `project_config.hpp`: configure aspects of the applications
  code such as, log level, number of fatfs drives, whether or not to store error
  messages within exception objects and more. See
  [`config.hpp`](https://github.com/SJSU-Dev2/SJSU-Dev2/blob/master/library/config.hpp)
  for the complete list of configuration options. See `hello_world/`'s
  [`project_config.hpp`](https://github.com/SJSU-Dev2/SJSU-Dev2/blob/master/projects/hello_world/project_config.hpp)
  for an example of how to use this.

### Build Folder

The build folder contains all of the files generated from running make commands.
For example, running `make application` will result in `.o` (object files), `.a`
static library files, `.bin` (binary file), `.elf` (extended link file) and more
build artifacts being generated.
