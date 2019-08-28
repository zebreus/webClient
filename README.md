# WebClient
The web based client for the certificate generator.

## Installation
Just run `make all`, to generate the html js and wasm files to `output/`.

## Usage
Copy the content of `output/` to a webserver. The main file is `webclient.html`.
If you have emrun installed, you can just run `make execute`, to start a webserver.

## Requirements
### For building from source
 - docker
 - make
### For building from source without docker
- emscripten 1.38.30
- thrift 0.13.0
- boost (only system headers are required)
- make
### For execution
- a webserver
- a modern webbrowser
