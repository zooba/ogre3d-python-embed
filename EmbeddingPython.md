# Embedding Python

In this demo, we have embedded the Python 3.6 runtime into the sample. This document outlines the configuration settings that are required for this, and discusses alternative approaches.

# Python installation

Depending on your application, there are three feasible ways to obtain the Python runtime.

In this section, we will discuss each approach and when you may consider using it.

## Development Kit

The installer available from [python.org](https://www.python.org/) provides the full development kit for Python, including documentation, test suite and debugging files. It will be installed to a common location on a user's machine, and can be located on Windows using the registry (see [PEP 514](https://dev.python.org/peps/pep-0514.html) for details).

Using the full installation is recommended only for developers, as it requires extra configuration in order to work well. For example, this sample relies on an existing Python 3.6 64-bit installation, which most regular users will not have and should not have to obtain.

## Nuget.org Package

[Nuget](https://nuget.org) is a package management tool primarily for .NET but with good support across the Microsoft ecosystem. There are four Python packages available: [python](https://www.nuget.org/packages/python/), [pythonx86](https://www.nuget.org/packages/pythonx86/), and two packages for legacy Python 2.7 versions. These are installed using the Nuget command line tool or a build system, and will be installed to a private directory that is not registered globally.

Using the Nuget packages is recommended for builds and build systems. The package only includes the minimum files required to build and run most Python applications, and also includes `pip` and related tools so that extra dependencies can be installed. Recent version of the packages also include a `build\native\python.props` file that can be imported into an [MSBuild project](https://docs.microsoft.com/visualstudio/msbuild/msbuild).

## Embeddable Package

As well as the development kit, the [python.org](https://www.python.org/) site also provides an _embeddable distribution_ of Python 3.5 and later. This package is heavily stripped down to reduce the size to approximately 10MB, and only includes the standard library. It is intended to be copied into your own application and distributed as part of it, and so it is provided as a simple ZIP file with no actual installer.

Using the embeddable package is recommended when you are distributing an application that includes Python. While there is a `python.exe` in the package, it is expected that most applications will not require this and will directly use the `python3#.dll` module.

# Configuring embedded Python

For this sample, we required the full development kit installation of Python, including debugging symbols (which are not installed by default). This provides the best experience when developing and debugging the application in [Visual Studio 2017](https://visualstudio.com/vs/python).

In this section, we will show the settings used in Visual Studio to allow Python to be used directly from the development kit. You can see these settings by looking at the files in this repository. We will also describe how we would use the other two packages to build and distribute the sample.

## Building and debugging with the Development Kit

We are using an MSBuild project (`*.vcxproj`), and so most of the required configuration can be extracted into the [`python.props`](python.props) file. This file is generic and may be freely used in your own projects.

The first section of the file specifies two properties. `PythonCompany` refers to the [company](https://www.python.org/dev/peps/pep-0514/#company) value by which the installation is registered: for installs from [python.org](https://www.python.org/), this will be `PythonCore`. `PythonTag` is the [tag](https://www.python.org/dev/peps/pep-0514/#tag) based on the desired version number and the current platform (32-bit or 64-bit). These values are used to find an installation in the registry.

```xml
<PythonCompany>PythonCore</PythonCompany>
<PythonTag Condition="$(Platform) != 'x64'">3.6-32</PythonTag>
<PythonTag Condition="$(Platform) == 'x64'">3.6</PythonTag>
```

These are the only values that you should need to configure, as all the others are determined automatically. After importing this file, you will have the following macros available:

* `PythonHome`: the directory where Python is installed
* `PythonExe`: the full path to the interpreter
* `PythonDevVersion`: the full version number of the installation
* `PythonInclude`: the directory with Python's header files
* `PythonLibs`: the directory with Python's import libraries
* `PythonDebugSuffix`: filename suffix for debug builds (see below)
* `PythonDExe`: the full path to the debug build of the interpreter

For C++ projects, the include and libraries directories are automatically added to the build and no further configuration is needed for `#include <Python.h>` to be found and correctly compiled. Other project types may need to be configured manually using these macros.

When building an extension module, you should update your target extension to `.pyd` and add `$(PythonDebugSuffix)` to the end of the target name. This is required to use a debug build of Python to test your extension, which may provide a better experience. (If Python is built in debug mode, `import mymodule` will only find `mymodule_d.pyd`, and so `PythonDebugSuffix` will provide the `_d` when necessary.) The `PythonDExe` macro will point to the debug build of the interpreter (`python_d.exe`) if available, or the regular build if not.

Finally, we configure the two environment variables below in our project so that we can debug without having to copy our Python source files anyway, or copy the Python runtime into our output directory. Setting `PYTHONPATH` to our project directory allows us to keep (and edit) the Python sources in one place, while updating `PATH` to include `$(PythonHome)` ensures that the correct `python3#.dll` file is found at runtime.

```xml
<LocalDebuggerEnvironment>PYTHONPATH=$(ProjectDir.TrimEnd(`\`))
PATH=%PATH%;$(PythonHome.TrimEnd(`\`))</LocalDebuggerEnvironment>
```

With these variables set, you are now ready to use F5 to launch your host application, or configure Visual Studio to launch `$(PythonDExe)` with commands to import and test your extension module.

## Building with the Nuget package

The next step in our application is setting up continuous builds on a service such as [Visual Studio Team Services](https://aka.ms/vsts). When working with distributed build systems, it is important to be self-sufficient with regards to dependencies - assuming that the machine used for the build will have the correct versions of what we need is a risk. Luckily, by using the Nuget package for Python, we can avoid this risk.

Since we are using 64-bit Python, we will install the [python](https://www.nuget.org/packages/python/) package as one of our build steps. By specifying the `-OutputDirectory` argument, we know where it will be installed. (Alternatively, on VSTS, we could use the [Install Python](https://marketplace.visualstudio.com/items?itemName=stevedower.python) task from the Python build tools extension.)

```
nuget install python -Version 3.6.6 -OutputDirectory "$(Build.BinariesDirectory)"
```

Now we have a `$(Build.BinariesDirectory)\python.3.6.6\build\native\python.props` file that we can import instead of the one we used earlier for debugging. This properties file is simpler, as it does not support debugging, but still automatically configures builds to use the headers and libraries for the package. To select the correct file, we use a `PythonProps` property (see [include.props](include.props)) that we leave unset when debugging, but override for full builds.

```xml
<Import Project="$(PythonProps)" Condition="$(PythonProps) != ''" />
<Import Project="python.props" Condition="$(PythonProps) == ''" />
```

```
msbuild PythonCharacter.vcxproj 
    /p:Platform=x64
    /p:Configuration=Release
    /p:PythonProps="$(Build.BinariesDirectory)\python.3.6.6\build\native\python.props"
```

## Bundling the Embeddable Package

Building against the Nuget package is an easy way to ensure the correct headers are available, but when we distribute this application we do not want to include those. In fact, we want Python to be totally hidden, as it is just an internal dependency. This is the use of the embeddable package.

With the debug install, we had to configure `PATH` and `PYTHONPATH` in order for our application to locate the necessary files. We also had access to all installed packages, but to distribute an application we will need to include those as well. Here is an outline of how to configure this using the embeddable package:

1. Extract the package into your application directory

Since our application depends on `python36.dll`, we want to make sure that DLL is in the same directory as our executable. Provided we have no naming collisions, it is easiest to extract the embeddable ZIP into our output directory as part of build.

Once this is done, there is no more need to update `PATH` in order to locate our dependencies. If you want to keep the Python runtime in its own directory, you will need to delay load the Python DLL and use the [`AddDllDirectory`](https://msdn.microsoft.com/library/windows/desktop/hh310513(v=vs.85).aspx) function to add its directory to the search path before calling any of its functions.

2. Remove unused files from the embeddable package

Depending on your application, you may not require the entire contents of the package. In our case, we only need `python36.dll` and `python36.zip` (which contains the standard library), so the rest can be removed.

3. Install dependencies

Any Python dependencies that your application has need to be installed as part of your build. However, the embeddable package does not include (or support!) `pip` and `setuptools`. The best way to install packages is to use a Nuget package or full install _of the same version of Python_, and pass the `--target` parameter:

```
$(Build.BinariesDirectory)\python.3.6.6\tools\python.exe -m pip install 
    --target "$(Build.BinariesDirectory)\bin\libraries"
    -r requirements.txt
```

Note that this works best when the target directory is empty. You may also want to consider installing to a temporary directory and converting each library into a ZIP file, rather than distributing many individual Python files.

4. Update `python36._pth`

The `._pth` file is used to put Python into isolated mode. This prevents interference from other installs of Python by ignoring all the regular environment variables and registry entries. Each line of this file contains a relative path to a directory or ZIP file that should be added to `sys.path`. If you keep your Python sources in a ZIP file or a subdirectory of your application, it will need to be specified here. Even `.` is not assumed, so if you have modules alongside `python36.dll` then adding `.` on its own line is required.

You can optionally add `import site` as a line in this file (no other imports are supported). This is useful in limited circumstances, such as resolving regular `.pth` files, but generally an embedded application does not require it and you can improve performance by omitting this line.

Also note that `._pth` files are found by looking at both the executable name and the Python DLL name. If your application has multiple entry points, each one may use different search paths by using a separate `name._pth` file. Having a `._pth` file removes the need to set `PYTHONPATH` before running your application.

5. Create your installer package

However you choose to distribute the application, is it important to include _all_ files that have come from the embeddable ZIP and any installed packages. These now belong to your application and should be installed, managed, and updated as your own.

For a native application, it is likely that you are already including the required C Runtime dependencies needed by Python, but if not then you should ensure your installer includes `vcruntime140.dll` (for current Python versions) and installs the [Universal C Runtime](https://docs.microsoft.com/cpp/c-runtime-library/crt-library-features) when required (prior to Windows 10).

# Summary

In this document we've looked at the configuration required to embed and bundle Python into an application on Windows. This is useful when distributing an application that uses Python as an internal dependency and you do not want to require users to install or configure it themselves. It also provides stability for your application, giving you full control over the version of Python that is used.

Our recommendations are:
* use a full Python install from [python.org](https://www.python.org/) with debug symbols while developing
* use a package from [nuget.org](https://nuget.org/) while building (for example, on [VSTS](https://aka.ms/vsts))
* put the embeddable package from [python.org](https://www.python.org/) in your application when distributing
