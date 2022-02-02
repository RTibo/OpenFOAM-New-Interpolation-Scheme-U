/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::UInterpolationScheme

Description
    UInterpolationScheme differencing scheme class.

SourceFiles
    UInterpolationScheme.C

\*---------------------------------------------------------------------------*/

#ifndef UInterpolationScheme_H
#define UInterpolationScheme_H

#include "surfaceInterpolationScheme.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class UInterpolationScheme Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class UInterpolationScheme
:
    public surfaceInterpolationScheme<Type>
{
    // Private Data

    scalar z0_ = 0;

public:

    //- Runtime type information
    TypeName("UInterpolationScheme");


    // Constructors

        //- Construct from Istream.
        UInterpolationScheme
        (
            const fvMesh& mesh,
            Istream& is
        )
        :
            surfaceInterpolationScheme<Type>(mesh)
        {
            z0_ = readScalar(is);
        }

        //- Construct from faceFlux and Istream
        UInterpolationScheme
        (
            const fvMesh& mesh,
            const surfaceScalarField& faceFlux,
            Istream& is
        )
        :
            surfaceInterpolationScheme<Type>(mesh)
        {}

    // Member Functions

        //- Return the interpolation weighting factors
        virtual tmp<surfaceScalarField> weights
        (
            const GeometricField<Type, fvPatchField, volMesh>&
        ) const
        {
            NotImplemented;

            return tmp<surfaceScalarField>(nullptr);
        }

        //- Return the face-interpolate of the given cell field
        virtual tmp<GeometricField<Type, fvsPatchField, surfaceMesh>>
        interpolate
        (
            const GeometricField<Type, fvPatchField, volMesh>& vf
        ) const
        {
            const fvMesh& mesh = vf.mesh();

            tmp<GeometricField<Type, fvsPatchField, surfaceMesh>> tvff
            (
                new GeometricField<Type, fvsPatchField, surfaceMesh>
                (
                    IOobject
                    (
                        "UInterpolationScheme::interpolate(" + vf.name() + ')',
                        mesh.time().timeName(),
                        mesh
                    ),
                    mesh,
                    vf.dimensions()
                )
            );
            GeometricField<Type, fvsPatchField, surfaceMesh>& vff = tvff.ref();

            // computing interpolation on the internal faces-------------------
            forAll(mesh.Cf(), facei)
            {
                // getting owner and neighbour cell indices first
                label owner_id = mesh.faceOwner()[facei];
                label neighbour_id = mesh.faceNeighbour()[facei];

                // finding which cell is to be P and N based on height
                scalar z_owner = mesh.C()[owner_id].z();
                scalar z_neighbour = mesh.C()[neighbour_id].z();

                label P_id, N_id;

                if (z_owner < z_neighbour)
                {
                    P_id = owner_id;
                    N_id = neighbour_id;
                }
                else
                {
                    N_id = owner_id;
                    P_id = neighbour_id;
                }

                // getting Z coordinates of P and N cells & n face
                scalar Z_N = mesh.C()[N_id].z()+z0_;
                scalar Z_P = mesh.C()[P_id].z()+z0_;
                scalar Z_n = mesh.Cf()[facei].z()+z0_;
                // scalar Z_N = mesh.C()[N_id].z();
                // scalar Z_P = mesh.C()[P_id].z();
                // scalar Z_n = mesh.Cf()[facei].z();

                // calculating beta factor
                scalar numerator = Foam::log(Z_N/Z_n);
                scalar denominator = Foam::log(Z_N/Z_P);

                // adjusting for zero-error at either num or denom
                // which occurs for cells at same level on the sides
                // if num or denom = 0; then alpha = 1, so that the face will
                // take the U_P as its value directly
                scalar alpha(1.0);
                if(numerator != 0 || denominator != 0)
                    alpha = numerator/denominator;

                // interpolating the incoming field
                // note that the input field (U in our case) is vf here
                // and the interpolated surface field is vff
                vff[facei] = alpha*vf[P_id] + (1-alpha)*vf[N_id];

                // thats all, our work is done

            }
            //-----------------------------------------------------------------
            // below is the stuff that takes care of boundary values to the
            // given input field, which we need not touch as it is already
            // been taken care

            typename GeometricField<Type, fvsPatchField, surfaceMesh>::
                Boundary& bff = vff.boundaryFieldRef();

            forAll(bff, patchi)
            {
                const fvPatchField<Type>& pf = vf.boundaryField()[patchi];
                Field<Type>& pff = bff[patchi];

                if (pf.coupled())
                {
                    tmp<Field<Type>> tpif(pf.patchInternalField());
                    const Field<Type>& pif = tpif();

                    tmp<Field<Type>> tpnf(pf.patchNeighbourField());
                    const Field<Type>& pnf = tpnf();

                    forAll(pff, i)
                    {
                        pff[i] = max(pif[i], pnf[i]);
                    }
                }
                else
                {
                    pff = pf;
                }
            }

            return tvff;
        }

    // Member Operators

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //

