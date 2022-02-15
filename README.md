# OpenFOAM - New Interpolation Scheme U

# Introduction

This is an attempt to create a new OF interpolation scheme for field U.

The scope of this work is to use the new interpolation scheme for a Neutral Atmospheric Boundary Layer application
with the k-e turbulence model.

The code compile successfully, but when it use during a simulation the results are not affected by the scheme.

It seem that the interpolation scheme don't work properly and I can't find where the issue is.

The version of OpenFOAM used is OF v2012.

Link of the paper (equation 44 & 45): https://onlinelibrary.wiley.com/doi/10.1002/fld.2709

Programmer: https://github.com/Ramkumar47

# Equation

The interpolation scheme use for this project is:

![image](https://user-images.githubusercontent.com/64756528/153983776-8a9a24c3-ee45-4c35-b94b-9789359ea2ce.png)

Where,

![image](https://user-images.githubusercontent.com/64756528/153983803-5672e73e-c57c-4700-bd3e-d50bfc63ee15.png)


# 1D Case

The goal of the interpolation scheme is to apply the interpolation calculation on the component 'u' of the velocity.
That being said, the value 'Un' at the north of the cell 'Zn' is where the scheme should be apply. The folder 'Test Case' has two cases folder that include the results for each individual cases, and the results compiled in one tabulated file with graphs. The case '1D_ke' is the results obtain with the turbulence model k-epsilon alone and the case '1D_IntU' is the case where the interpolation scheme is apply. The mesh is a 1D mesh that include 150 cells in the 'Z' direction. 

![image](https://user-images.githubusercontent.com/64756528/153982544-8ae44806-2e3b-4d31-804b-e795111eed10.png)
