# Simulit

A simple desktop application for running and testing simulations.
Built with C++20, Qt 6.8 (QtQuick), targeting Windows 11 / MSVC 2022.

The app automatically discovers simulation modules provided as DLL plugins.
Just drop a DLL into the simulations/ folder next to the executable.

## Features

- QtQuick UI with simulation list, properties panel, and statistics view
- Simulations loaded as external DLL modules
- API for creating custom simulations (api/)
- Automatic DLL detection at startup
- Built-in controls for control simulation runs
- controlling simulation using parameters and collecting statistics 

## Project Structure

```bash
/api/            → public API for simulation plugins
/simulations/    → DLLs loaded by the application
/src/            → application source code (C++ + QML)
```

## Creating a Simulation (DLL Plugin)

1. Include the API from the api/ folder (see example simulations at simulations/ directory)
2. Implement the required simulation interface
3. Build your project as standalone DLL
4. Place the DLL into the simulations/ folder next to the executable
5. The application will detect it automatically


## Running the Application

1. Build the project using MSVC 2022 and Qt 6.8.
2. Launch the app — available simulations will appear in the menu.


## License
MIT License — free to use, modify, and share.
