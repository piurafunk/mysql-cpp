from conan import ConanFile


class MysqlCpp(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    python_requires = "buck_generator/0.1"
    header_namespace = "MysqlCpp"

    def requirements(self):
        self.requires("openssl/3.0.5", libs=True)
        self.requires("catch2/2.13.10", libs=True)

    def generate(self):
        mygenerator = self.python_requires["buck_generator"].module.BuckGenerator(self)
        mygenerator.generate()
