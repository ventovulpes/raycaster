# Raycaster
An implementation of a Wolfenstein-style raycaster using C.

## Usage
* Default map: `make run` or `make demo`
* Specify map: `make run FILE=<map_file_path>`
* Build to `./build/raycaster`: `make` or `make build`

## Map File
A .txt file with lines of equal length.  
Can only contain these symbols:
* `X`: default wall
* `R`: red wall
* `G`: green wall
* `B`: blue wall
* `.`: empty
* `P`: player (map must contain exactly one P)