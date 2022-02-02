# OpenFOAM-New-Interpolation-Scheme-U
This is an attempt to create a new OF interpolation scheme for field U.

The scope of this work is to use the new interpolation scheme for a Neutral Atmospheric Boundary Layer application
with the k-e turbulence model.

The code compile successfully, but when it use during a simulation the results are not affected by the scheme.

It seem that the interpolation scheme don't work properly and I can't find where the issue is.

The version of OpenFOAM used is OF v2012.

Link of the paper (equation 44 & 45): https://onlinelibrary.wiley.com/doi/10.1002/fld.2709

Programmer: https://github.com/Ramkumar47
