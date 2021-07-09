# GBGO Emulator

## In development

A Game Boy emulator built with WebAssembly. Heavily based on [Jonazan2s's PatBoy](https://github.com/Jonazan2/PatBoy).

It supports the use of a xbox one controller 🎮.

![Screenshot](https://user-images.githubusercontent.com/70603530/125121693-47559380-e0c2-11eb-9ef0-9fd06dd6164a.png)

### TODO

- Cleanup the javascript code (**High**)
- Cleanup the c++ code (**High**)
- Implement the APU (**Medium**)
- Add debug stats/options to the interface (**Medium**)
- Support Mobile devices (**Medium**)
- Support Gameboy Color (**Low**)

## Installation

You need Emscripten to compile the c++ source and Nodejs to use the interface.

Execute `compile` in the root folder to compile the source with Emscripten. Alternatively, the command  `compile -d` can be used to ensure that compiled code contains enough information for debugging. 

With Nodejs installed, you can fetch the interface dependencies with `npm isntall` and use the command `npm start` to start the express server at localhost:3000.

It is possible to compile a native executable version of the emulator using cmake. To do so, a cmakelist file is located in the src folder. It was tested using visual studio 2019.

## Contributing

New features/pull requests are always welcome.

## Credits

Thanks to [Jonazan2s's PatBoy](https://github.com/Jonazan2/PatBoy) for providing a lot of the c++ code needed for this emulation project.

## License

[MIT](https://choosealicense.com/licenses/mit/)