from conans import ConanFile, CMake 
import multiprocessing

class QEOrmConan(ConanFile):
    name = "QEOrm"
    version = "0.1.0"
    requires = "QEAnnotation/0.1.0@fmiguelgarcia/stable"
    settings = "os", "compiler", "build_type", "arch"
    license = "https://www.gnu.org/licenses/lgpl-3.0-standalone.html"
    generators = "cmake"
    url = "ssh://miguel@migroslinx.no-ip.org/home/miguel/git/QE/QEOrm"
    description = "ORM library based on Annotations"

    def source(self):
        self.run("git clone %s" % self.url) 

    def build(self):
        cmake = CMake( self.settings)
        self.run( "cmake %s/QEOrm %s" % (self.conanfile_directory, cmake.command_line))
        self.run( "cmake --build . %s"  % cmake.build_config ) 

    def package(self):
        self.copy( pattern="*.hpp", dst="include/QEOrm/", src="QEOrm/src")
        self.copy( pattern="LICENSE.LGPLv3", dst="share/QEOrm")
        self.copy( pattern="libQEOrm.so*", dst="lib", src="src")

    def package_info(self):
        self.cpp_info.libs.extend(["QEOrm"])
        self.cpp_info.includedirs.extend(["include/QEOrm"])
