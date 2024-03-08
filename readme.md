## Dependencies

Before building the project, make sure you have the following dependencies installed:

```bash
sudo apt install g++ build-essential cmake libopengl0 libopengl-dev libglfw3 libglfw3-dev
```

## Submodules

This project uses Git submodules to manage external dependencies. After cloning the repository, make sure to initialize and update the submodules using the following commands:

```bash
git submodule init
git submodule update
```