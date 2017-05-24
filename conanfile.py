from conans import ConanFile, CMake 

class QEOrmConan(ConanFile):
    name = "QEOrm"
    version = "1.0.0"
    requires = "QEEntity/1.0.0@fmiguelgarcia/stable"
    settings = "os", "compiler", "build_type", "arch"
    license = "https://www.gnu.org/licenses/lgpl-3.0-standalone.html"
    generators = "cmake"
    url = "https://github.com/fmiguelgarcia/QEOrm.git"
    description = "ORM library based on Annotations"
    exports_sources = ["src/*", "test/*", "tools/*", "CMakeLists.txt"]

    def build(self):
        cmake = CMake( self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy( pattern="*.hpp", dst="include/qe/orm/", src="src/qe/orm")
        self.copy( pattern="LICENSE.LGPLv3", dst="share/qe/orm")
        self.copy( pattern="libQEOrm.so*", dst="lib", src="src/qe/orm",
                links=True)
        if self.settings.os == "Windows":
            libNames = ["QEOrm", "libQEOrm"]
            libExts = [".dll", ".lib", ".dll.a", ".pdb"]
            for libName in libNames:
                for libExt in libExts:
                    filePattern = "**/" + libName + libExt
                    self.copy( pattern=filePattern, dst="lib", src="src/qe/orm", keep_path=False)


    def package_info(self):
        self.cpp_info.libs.extend(["QEOrm"])
