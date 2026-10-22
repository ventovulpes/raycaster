# Raycaster
An implementation of a Wolfenstein-style raycaster using C.

## Usage
* Default map: `make run`
* Specify map: `make run FILE=<map_file_name>`

## Map File
A .txt file with lines of equal length.  
Can only contain these symbols:
* `X`: default wall
* `R`: red wall
* `G`: green wall
* `B`: blue wall
* `.`: empty
* `P`: player (map must contain exactly one P)