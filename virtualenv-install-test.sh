# Remove the existing virtual environment if it exists
rm -rf bgslibrary_test_env

# Create a new virtual environment called bgslibrary_test_env using Python3
python3 -m venv bgslibrary_test_env

# Activate the newly created virtual environment
source bgslibrary_test_env/bin/activate

# Upgrade pip and install required packages numpy and OpenCV
python -m pip install --upgrade pip
python -m pip install numpy
python -m pip install opencv-python

# Build and install the package using the setup.py script
# python setup.py build
# python setup.py install

# Set the PYTHONPATH environment variable to the build directory to access the installed library
# The following line is for Linux
# export PYTHONPATH=$PYTHONPATH:$PWD/build/lib.linux-x86_64-cpython-38
# The following line is for Mac
# export PYTHONPATH=$PYTHONPATH:$PWD/build/lib.macosx-11-x86_64-cpython-39

# Install the pybgs directly from PyPI
python -m pip install pybgs

# Run demo.py and demo2.py to verify the package installation
python demo.py
python demo2.py
