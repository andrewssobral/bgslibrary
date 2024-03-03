# Remove the existing virtual environment if it exists
rm -rf bgslibrary_env

# Create a new virtual environment called bgslibrary_env using Python3
python3 -m venv bgslibrary_env

# Activate the newly created virtual environment
source bgslibrary_env/bin/activate

# Upgrade pip and install required packages numpy and OpenCV
python -m pip install --upgrade pip
python -m pip install wheel setuptools
python -m pip install numpy
python -m pip install opencv-python

# Remove any existing build directory
rm -rf build/*

# Build and install the package using the setup.py script
python setup.py build
python setup.py install

# Set the PYTHONPATH environment variable to the build directory to access the installed library
# The following line is for Linux
# ubuntu 20
export PYTHONPATH=$PYTHONPATH:$PWD/build/lib.linux-x86_64-cpython-38
# ubuntu 22
export PYTHONPATH=$PYTHONPATH:$PWD/build/lib.linux-x86_64-cpython-310
# ubuntu 24
export PYTHONPATH=$PYTHONPATH:$PWD/build/lib.linux-x86_64-cpython-312
# The following line is for Mac
export PYTHONPATH=$PYTHONPATH:$PWD/build/lib.macosx-11-x86_64-cpython-39

# Run demo.py and demo2.py to verify the package installation
python demo.py
python demo2.py

# Install the Twine package for uploading the distribution packages
python -m pip install twine

# Remove any existing build directory
rm -rf build/*

# Build a Wheel distribution package for the project
python setup.py bdist_wheel

# Upload any generated Wheel distribution packages using Twine
twine upload dist/*.whl

# Create a source distribution package for the project
python setup.py sdist

# Upload the generated source distribution package using Twine
twine upload dist/pybgs-*.tar.gz
