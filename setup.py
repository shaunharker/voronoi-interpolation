import sys

try:
    from skbuild import setup
    import nanobind
except ImportError:
    print("The preferred way to invoke 'setup.py' is via pip, as in 'pip "
          "install .'. If you wish to run the setup script directly, you must "
          "first install the build dependencies listed in pyproject.toml!",
          file=sys.stderr)
    raise

setup(
    name="voronoi_interpolation",          # <- The package name (e.g. for PyPI) goes here
    version="0.0.1",        # <- The current version number.
    author="Shaun Harker",
    author_email="sharker81@gmail.com",
    description="Image interpolation using Voronoi cell averages drawn in Delaunay triangulation",
    long_description="Image interpolation using Voronoi cell averages drawn in Delaunay triangulation",
    long_description_content_type='text/markdown',
    url="https://github.com/username/repository_name",
    python_requires=">=3.8",
    license="BSD",
    packages=['voronoi_interpolation'],     # <- The package will install one module named 'my_ext'
    package_dir={'': 'src'}, # <- Root directory containing the Python package
    cmake_install_dir="src/voronoi_interpolation", # <- CMake will place the compiled extension here
    include_package_data=True
)
