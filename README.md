# Remote-Gamepad

A tool for using gamepad for PC remote session.

### How it is implemented
Local machine's gamepad input is being captured through XInput, then being serialized and sent to the remote machine through TCP, and then applied there through the [ViGEmClient API.](https://github.com/ViGEm/ViGEmClient)

### Preconditions
- client and server machines are both x64 and Windows 7/8/10.
- if you want to open the solution you need to have Visual Studio 2019.
- [ViGEmBus driver](https://github.com/ViGEm/ViGEmBus) has to be installed on server machine to make it work.

### How to make it work
- First of all, open the config file (config.json) and write there the port and IP you want to use. DNS address is allowed.
- Then, place one copy of the application on the server machine and other on the client machine. Make sure they both have the same config file.
- Run server program, see the output.
- Run client program. You will see if the connection was succesful.
- Enjoy your remote gameplay.

### How to set up the solution
- clone repository with all submodules
- go to vcpkg, run "boostrap-vcpkg.bat" to install the Microsoft Package Manager.
- now, using vcpkg, install "nlohmann-json:x64-windows" and "boost-asio:x64-windows".
- run "vcpkg integrate project" command and follow the instructions. Don't forget to install the package for all projects in solution (you will have to run this command 3 times).
- compile and link.

### 3rd parties
- [ViGEmBus driver](https://github.com/ViGEm/ViGEmBus)
- [ViGEmClient API](https://github.com/ViGEm/ViGEmClient)
- [nlohmann-json](https://github.com/nlohmann/json)
- [boost-asio](https://github.com/boostorg/asio)
