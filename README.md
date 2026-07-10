This repo is for learning purposes and exploring CPP in a ROS context. All resources can be found at learncpp.com.

If cloning this repo onto a new machine, run the following command in order to get the setup for clangd: 

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

To compile the executable, run the following command:

cmake --build build