# The KirHut Application Development Library

This project is primarily meant to provide common functionality for all of the KirHut software applications, from the
KirHut Security Suite to any embedded or command line tools KirHut creates. This library provides a foundation with
which all KirHut software can use to build the software to an expected standard. This library has the option to depend
on Qt, at least version 5.15 but it supports 6.5 and beyond easily. The Qt libraries are used with dynamic linkage to
ensure compliance with whichever license is used to build Qt (GPL or LGPL). Qt, when built with this library, is used
under the terms of the LGPL with the standard distributions as published by the KirHut Software Company.

This project is also intended to be useful for third party developers who would like a powerful foundation for
developing modern C++ software, and anyone who is developing a Qt application with a focus on security. As
such, you may find what this library provides to be useful. Using libKirHut should be as easy as downloading it,
building and installing it, then asking for the dependency in your CMake file. KirHut uses CPM for nearly all
dependencies, including libKirHut, which you may also use in your project for easy inclusion of this in your project.
With CPM, you should be able to do something like this:

```cmake
CPMAddPackage("gh:zekedragon/libKirHut")
target_link_libraries(myProject libKirHut)
```

This should do everything you need, now you can include libKirHut headers and build against the library.

## Documentation

As per the standard expected by KirHut software, libKirHut is meticulously documented. The documentation is written in
the header documents of the source primarily, with supplementation in the docs folder itself. The documentation is
generated using Doxygen and a Doxygen file can be found in the docs folder which will generate everything.

The programming guide in the documentation is essential reading for anyone who wants to modify KirHut software to the
standards the project requires.