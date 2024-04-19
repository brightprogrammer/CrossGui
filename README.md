# CrossWindow

[![](https://img.shields.io/badge/Discord-7289DA?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/https://discord.gg/J9b45jbAdH) <a href="https://www.buymeacoffee.com/brightprogrammer" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="28" width="105"></a>

## Intro 

CrossGui is my solution for cross platform GUI application programming. I don't like current solutions,
and I wanna do something about it. CrossGui aims :
- to be completely free for life. 
- to be fast and simple to use
- to provide custom themeing support
- to provide a dynamic ui layout mechanism, much like HTML
- to not compromise on the quality, ever!

CrossGui is under development for now, and it'll take some time to make it usable, and much more to
make it stable. CrossGui depends on [CrossWindow](https://github.com/brightprogrammer/CrossWindow)
that is also written by me, to achieve cross platform support.

## Build 

To build and install CrossWindow as a library on your system, follow the following steps :
- Create a build directory where you want to build the library. I usually name this as `Build` inside the project root itself.
- Go inside `Build` directory and open a terminal, or you can follow any other method to go to that directory inside the terminal.
- Run the following command :
    - If you have Ninja installed on your system, then use it, it's better : `cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release`
    - If you don't have Ninja installed, then we'll just use GNU Make : `cmake .. -DCMAKE_BUILD_TYPE=Release`
- The last step will generate build files needed for building the project, and now you can run :
    - `ninja` if you have Ninja installed
    - `make -j${nproc}` if you have make installed. You can run just `make` as well. The only difference is number of threads provided for building the project.
- The last step will build the project, and now you can run it (for now) using `bin/main`
