# Ogre3d Embedding Demo

This demo embeds a Python runtime into a sample in the ogre3d engine.

Ensure you have installed Visual Studio 2017 with the Python workload, and select the *Python native development* and *Python 3.6 (32-bit)* options.

After cloning (with submodules), run [get_externals.ps1](get_externals.ps1) with PowerShell to install dependencies. This may require you to download and install some packages manually (links are provided).

Open the solution file and press F5 to build and run. Once in the sample, you can Alt+Tab back into Visual Studio, make modifications to `AI.py`, and press F5 in the game again to load your changes.

More detailed instructions will come in the future.

# How does this work

See [Embedding Python](EmbeddingPython.md) for a discussion of embedding a Python runtime in a C++ application in Visual Studio 2017.
