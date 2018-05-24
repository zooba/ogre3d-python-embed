# Tools

Some extra files that may be useful when running this demo.

## Visualizers

The `pybind.natstepfilter` and `pybind.natjmc` files suppress many of the `pybind11` frames when debugging.

When *Just my Code* is enabled, the `natjmc` file will mark `pybind11` calls as external code that will only appear in the call stack when *Show External Code*.

When stepping, the `natstepfilter` file causes Visual Studio to automatically step over `pybind11` frames either directly into Python or C++. Note that stepping out of a function will still step through `pybind11` frames.

To use them, copy both files to `%UserProfile%\Documents\Visual Studio 2017\Visualizers` and restart debugging.
