# HK-minigame-solver
C++ script for solving mini-games from Hamster Kombat automatically. Uses 2 local html files to enter initial grid data and visualize the solution.

How to use.
Before the first launch, you must specify the following parameters in order in the "config.exe" file:
    1) the size of the square grid (by default 6 by 6)
    2) the maximum number of steps in the solution (by default 36)
    3) the path to the browser downloads folder
    4) the winning key position (by default 4252).

After setting the required values ​​in the configuration, you can run the program:
    1) run the "main.exe"
    2) the script automatically runs the "picker.html" file where you can set the obstacles like this: click the start point, then click the end point, and the obstacle will appear automatically. the first obstacle is always the key obstacle. if you make a mistake by making the wrong obstacle, then reload the browser page and start again
    3) аfter creating the required grid, click on the button below and data will be automatically send to the script
    4) wait up to 5-10 seconds and script will automatically run the "visualiser.html" where you can see the solution by clicking two buutons below

After work, the script closes. If "visualiser.html" did not open automatically, then either the solution could not be found in the maximum number of steps set by config.ini (try increasing the value) or the solution does not exist (check if you have created the correct grid).

How to compile.
The compilation has no special features. To work, the compiled file must be in the same folder with the "config.ini" file and the "visualilser" folder. The script itself does not create “config.ini” or visualizer files. 

About visualiser folder.
"visualiser" folder contains Picker and Visualiser files which are can be used separately. Picker file used for creating required grid which is exporting as the txt after clicking the button, and Visualiser used to visualise the solution of the script. You can manually set the data for Visualiser by editing two first lines of the "code.js".
