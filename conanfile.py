from conans import ConanFile, CMake 
from conans.tools import os_info
import multiprocessing

class QEOrmConan(ConanFile):
    name = "QEOrm"
    version = "0.1.0"
    requires = "QEEntity/0.1.0@fmiguelgarcia/stable"
    settings = "os", "compiler", "build_type", "arch"
    license = "https://www.gnu.org/licenses/lgpl-3.0-standalone.html"
    generators = "cmake"
    url = "https://github.com/fmiguelgarcia/QEOrm.git"
    description = "ORM library based on Annotations"
    exports_sources = ["src/*", "test/*", "tools/*", "CMakeLists.txt"]

    def build(self):
        cmake = CMake( self.settings)
        parallel_build_flags = ("-- -j %d " % multiprocessing.cpu_count()) if os_info.is_linux else ""
        self.run( "cmake %s %s" % (self.conanfile_directory, cmake.command_line))
        self.run( "cmake --build . %s %s"  % (cmake.build_config, parallel_build_flags)) 

    def package(self):
        self.copy( pattern="*.hpp", dst="include/qe/orm/", src="src/qe/orm")
        self.copy( pattern="LICENSE.LGPLv3", dst="share/qe/orm")
        self.copy( pattern="libQEOrm.so*", dst="lib", src="src/qe/orm",
                links=True)

    def package_info(self):
        self.cpp_info.libs.extend(["QEOrm"])
