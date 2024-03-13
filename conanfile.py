from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class Conan(ConanFile):
    name = 'emu'
    version = '1.0.0'

    license = 'MIT'
    author = 'Julien Bernard jbernard@obspm.fr'
    url = 'https://gitlab.obspm.fr/cosmic/tools/emu'
    description = 'Set of utilities for C++, CUDA and python'

    settings = 'os', 'compiler', 'build_type', 'arch'

    options = {
        'shared'        : [True, False],
        'fPIC'          : [True, False],
    }

    default_options = {
        'shared'     : False,
        'fPIC'       : True,
    }

    def requirements(self):
        self.requires('fmt/10.2.1', transitive_headers=True)
        self.requires('open62541/1.3.9', transitive_headers=True)

    def layout(self):
        cmake_layout(self)

    generators = 'CMakeToolchain', 'CMakeDeps'

    def build(self):
        cmake = CMake(self)

        cmake.configure()
        cmake.build()
