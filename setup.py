"""
To build the bgslibrary:
    python setup.py build
To build and install:
    python setup.py install
To package the wheel (after pip installing twine and wheel):
    python setup.py bdist_wheel
To upload the binary wheel to PyPi
    twine upload dist/*.whl
To upload the source distribution to PyPi
    python setup.py sdist
    twine upload dist/pybgs-*.tar.gz
"""
import os
import re
import sys
import platform
import subprocess

from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

#import datetime
#now = datetime.datetime.now()
#
#pkg_properties={}
#with open('.properties') as fp:
#    for line in fp:
#        if '=' in line:
#            name, value = line.replace('\n','').split('=', 1)
#            if "SNAPSHOT" in value:
#                dev_version = "." + now.strftime("%y%m%d%H%M") + ".dev"
#                value = value.replace("-SNAPSHOT", dev_version)
#            pkg_properties[name] = value


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.10.0':
                raise RuntimeError("CMake >= 3.10.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        python_version = str(sys.version_info[0]) + "." + str(sys.version_info[1])
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DBGS_PYTHON_SUPPORT=ON',
                      '-DBGS_PYTHON_VERSION=' + python_version]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j8']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)
        print()

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name='pybgs',
    version='3.0.0',
    #version=pkg_properties["version"],
    author='Andrews Sobral',
    author_email='andrewssobral@gmail.com',
    url='https://github.com/andrewssobral/bgslibrary',
    license='MIT',
    description='Python wrapper for bgslibrary using pybind11 and CMake',
    long_description=long_description,
    long_description_content_type="text/markdown",
    ext_modules=[CMakeExtension('src')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
    #packages=find_packages('pybgs'),
    #package_dir={'':'pybgs'},
    keywords=['BGSLibrary', 'Background Subtraction', 'Computer Vision', 'Machine Learning'],
)
