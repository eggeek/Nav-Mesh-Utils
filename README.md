# Utilities for polyanya

Useful utilities for parsing gridmaps, polymaps and meshes.

The spec for polymaps and meshes are contained within `spec`, and some example
maps have been included in `maps`.

# Tools


`gridmap2poly`: Converts grid maps into the polymap format.
Suitable for loading into Fade2D.
Takes a grid map from stdin, and prints the polymap to stdout.

`poly2mesh`: Converts polymaps into the mesh format. Suitable for search
algorithms.
Takes a polymap from stdin, and prints the mesh to stdout.
Note that `poly2mesh` outputs the Fade2D license when the program runs.

`visualiser`: Writes a PostScript file representing an input polymap.
Takes a polymap file **as the first argument**.

`meshpacker`: Compresses a mesh into a "packed" mesh. As a mesh comprises only
numbers, and is whitespace agnostic (you can parse a mesh, even with all the
new lines replaced with spaces) even though the spec does not explicitly allow
it. This "packed" file format comprises 3-byte integers such that the decimal
number 20 is encoded as 0x000014. It also has a header of "pack".
Takes a mesh file **as the first argument**, and outputs a packed mesh with a
`.packed` extension.

`meshunpacker`: Uncompresses a packed mesh. Takes a packed mesh
**as the first argument**, and outputs the original mesh without the `.packed`
extension.

`meshmerger`: Greedily merges polygons of a mesh together. This prioritises
merging polygons together to get the biggest polygon together, while also
ensuring that any "dead end" polygons are not removed by this merging. You can
also supply the `--pretty` flag to make the output easier to read (while being
slightly non-conforming to the spec). Takes a mesh from stdin, outputs to
stdout.

`gridmap2rects`: Greedily constructs rectangles from a gridmap into a mesh.
Constructs the best rectangle based on the heursitic
`min(width, height) * area`. This is to weight square-like rectangles more than
very wide or long rectangles.
Takes a gridmap from stdin, and outputs a mesh to stdout.

Included is a basic `gridmap2mesh` script which converts a gridmap to a mesh,
and also strips the Fade2D license from `poly2mesh`.

# Compiling

This has been tested on:

- Arch Linux, kernel release 4.13.3-1-ARCH and g++ 7.2.0
- Arch Linux, kernel release 4.8.13 and g++ 6.3.1

Ensure you have [GMP](https://gmplib.org/) installed, and run `make all`.
All the utilities will be compiled.

If you do not use Linux and still wish to compile all the tools which do not
use Fade2D and GMP, running `make nofade` will compile all the tools except
for `visualiser` and `poly2mesh`.
