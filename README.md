# Simulit

A simple desktop application for running and testing simulations.
Built with C++20, Qt 6.10.1 (QtQuick), targeting Windows 11 / MSVC 2022.

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

Simulit window with two found simulations.
<img width="1077" height="746" alt="screenshot" src="https://github.com/user-attachments/assets/a8ac0d72-98ba-4b3c-aa0d-c7e8e378db16" />

Window with running simple simulation.
<img width="1076" height="742" alt="screenshot2" src="https://github.com/user-attachments/assets/a1f0f5a4-6abc-4ace-b150-075eadeaae23" />

View of finished animated simulation.
<img width="1307" height="750" alt="screenshot3" src="https://github.com/user-attachments/assets/d36e99f3-10e4-41eb-9627-74a98363475b" />


## License
MIT License — free to use, modify, and share.
