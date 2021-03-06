/*---------------------------------------------------------------------------*\
     ██╗████████╗██╗  ██╗ █████╗  ██████╗ █████╗       ███████╗██╗   ██╗
     ██║╚══██╔══╝██║  ██║██╔══██╗██╔════╝██╔══██╗      ██╔════╝██║   ██║
     ██║   ██║   ███████║███████║██║     ███████║█████╗█████╗  ██║   ██║
     ██║   ██║   ██╔══██║██╔══██║██║     ██╔══██║╚════╝██╔══╝  ╚██╗ ██╔╝
     ██║   ██║   ██║  ██║██║  ██║╚██████╗██║  ██║      ██║      ╚████╔╝ 
     ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝      ╚═╝       ╚═══╝  
 
 * In real Time Highly Advanced Computational Applications for Finite Volumes 
 * Copyright (C) 2017 by the ITHACA-FV authors
-------------------------------------------------------------------------------

License
    This file is part of ITHACA-FV

    ITHACA-FV is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ITHACA-FV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with ITHACA-FV. If not, see <http://www.gnu.org/licenses/>.

Class
    steadyNS

Description
    Reduction class for a Stationary Navier-Stokes problem.

SourceFiles
    steadyNS.C

\*---------------------------------------------------------------------------*/

/// \file
/// Header file of the steadyNS class.
/// \dir
/// Directory containing the header and source files for the steadyNS class.

#ifndef steadyNS_H
#define steadyNS_H
#include "fvCFD.H"
#include "singlePhaseTransportModel.H"
#include "turbulentTransportModel.H"
#include "simpleControl.H"
#include "pisoControl.H"
#include "fvOptions.H"
#include "reductionProblem.H"
#include "ITHACAstream.H"
#include "ITHACAforces.H"
#include "volFields.H"
#include <iostream>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

/*---------------------------------------------------------------------------*\
                        Class SteadyNS Declaration
\*---------------------------------------------------------------------------*/

/// Implementation of a parametrized full order <b> steady NS problem </b> and preparation of the the reduced matrices for the online solve.
/** In this class are implemented the methods for the offline solve of a steady NS problem
and the for the generation of the reduced matrices for subsequent online solve, this class is a son
of the reduction problem class */
class steadyNS: public reductionProblem
{


public:
    // Constructors    
    /// Null constructor
    steadyNS();

    /// Construct with argc and argv
    steadyNS(int argc, char *argv[]);
    ~steadyNS(){};

    // Member Functions
    /// List of pointers used to form the pressure snapshots matrix
    PtrList<volScalarField> Pfield;

    /// List of pointers used to form the velocity snapshots matrix
    PtrList<volVectorField> Ufield;

    /// List of pointers used to form the supremizer snapshots matrix
    PtrList<volVectorField> supfield;

    /// List of pointers used to form the pressure modes
    PtrList<volScalarField> Pmodes;

    /// List of pointers used to form the velocity modes
    PtrList<volVectorField> Umodes;

    /// List of pointers used to form the supremizer modes
    PtrList<volVectorField> supmodes;

    /// List of pointers used to form the list of lifting functions
    PtrList<volVectorField> liftfield;

    /// List of pointers used to form the homogeneous velocity snapshots
    PtrList<volVectorField> Uomfield;

    /// Tolerance for the residual of the stationary problems, there is the same tolerance for velocity and pressure
    scalar tolerance;

    /// Number of maximum iterations to be done for the computation of the truth solution
    scalar maxIter;

    /// Number of velocity modes used for the projection
    label NUmodes;

    /// Number of pressure modes used for the projection
    label NPmodes;

    /// Number of supremizer modes used for the projection
    label NSUPmodes;

    /** @name Reduced Matrices
    *
    */
    ///@{

    /// Diffusion term
    Eigen::MatrixXd B_matrix;

    /// Mass Matrix
    Eigen::MatrixXd M_matrix;

    /// Gradient of pressure matrix
    Eigen::MatrixXd K_matrix;

    /// Non linear term
    List <Eigen::MatrixXd> C_matrix;

    /// Div of velocity
    Eigen::MatrixXd P_matrix;

    /// Laplacian term PPE
    Eigen::MatrixXd D_matrix;

    /// Divergence of momentum PPE
    List <Eigen::MatrixXd> G_matrix;

    /// PPE BC1
    Eigen::MatrixXd BC1_matrix; 

    /// PPE BC2
    List <Eigen::MatrixXd> BC2_matrix; 

    /// PPE BC3
    Eigen::MatrixXd BC3_matrix;   
    ///@}
    //

    /// Viscous forces
    Eigen::MatrixXd tau_matrix;
    
    /// Pressure forces
    Eigen::MatrixXd n_matrix;


    // Other Variables
    /// Boolean variable to check the existence of the supremizer modes
    bool supex;

    /// Cunter used for the output of the full order solutions
    label counter = 1;

    // Dummy variables to transform simplefoam into a class
    /// Pressure field
    autoPtr<volScalarField> _p;

    /// Velocity field
    autoPtr<volVectorField> _U;

    /// Mesh
    mutable autoPtr<fvMesh> _mesh;

    /// simpleControl
    autoPtr<simpleControl> _simple;

    /// fvOptions
    autoPtr<fv::options> _fvOptions;

    /// Time
    autoPtr<Time> _runTime;

    /// Flux
    autoPtr<surfaceScalarField> _phi;

    /// Turbulence model
    autoPtr<incompressible::turbulenceModel> turbulence;

    /// Laminar transport (used by turbulence model)
    autoPtr<singlePhaseTransportModel> _laminarTransport;

    /// MRF variable
    autoPtr<IOMRFZoneList> _MRF;

    /// Reference pressure cell
    label pRefCell;

    /// Reference pressure value
    scalar pRefValue;

    /// continuity error
    scalar cumulativeContErr=0;

    // Functions
    /// Perform a truthsolve
    void truthSolve();

    /// Solve the supremizer problem
    void solvesupremizer();

    /// Perform a lift solve
    void liftSolve();

    // Wrapped Proj. Methods;    
    
    //--------------------------------------------------------------------------
    /// Project using the Poisson Equation for pressure
    ///
    /// @param[in]  folder     The folder used to save the reduced matrices.
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    /// @param[in]  NSUPmodes  The number of supremizer modes, 0 by default and it must remain always zero.
    ///
    void projectPPE(fileName folder, label NUmodes, label NPmodes, label NSUPmodes = 0);

    //--------------------------------------------------------------------------
    /// Project using a supremizer approach
    ///
    /// @param[in]  folder     The folder used to save the reduced matrices.
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    /// @param[in]  NSUPmodes  The number of supremizer modes.
    ///
    void projectSUP(fileName folder, label NUmodes, label NPmodes, label NSUPmodes);

    //--------------------------------------------------------------------------
    // Projection Methods Momentum Equation    
    /// Diffusive Term
    ///
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    /// @param[in]  NSUPmodes  The number of supremizer modes.
    ///
    /// @return     reduced matrix in Eigen::MatrixXd format for the diffusion term.
    ///
    Eigen::MatrixXd diffusive_term(label NUmodes, label NPmodes, label NSUPmodes);

    //--------------------------------------------------------------------------
    /// Gradient of pressure
    ///
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    /// @param[in]  NSUPmodes  The number of supremizer modes.
    ///
    /// @return     reduced matrix in Eigen::MatrixXd format for the Gradient of pressure term.
    ///
    Eigen::MatrixXd pressure_gradient_term(label NUmodes, label NPmodes, label NSUPmodes);

    //--------------------------------------------------------------------------
    /// Convective Term
    ///
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    /// @param[in]  NSUPmodes  The number of supremizer modes.
    ///
    /// @return     reduced third order tensor in List <Eigen::MatrixXd> format for the convective term.
    ///
    List < Eigen::MatrixXd >  convective_term(label NUmodes, label NPmodes, label NSUPmodes);

    //--------------------------------------------------------------------------
    /// Mass Term
    ///
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    /// @param[in]  NSUPmodes  The number of supremizer modes.
    ///
    /// @return     reduced matrix in Eigen::MatrixXd format for the mass matrix.
    ///
    Eigen::MatrixXd  mass_term(label NUmodes, label NPmodes, label NSUPmodes);

    // Projection Methods Continuity Equation

    //--------------------------------------------------------------------------
    /// Divergence Term (supremizer approach)
    ///
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    /// @param[in]  NSUPmodes  The number of supremizer modes.
    ///
    /// @return     reduced matrix in Eigen::MatrixXd format for the divergence term.
    ///
    Eigen::MatrixXd divergence_term(label NUmodes, label NPmodes, label NSUPmodes);

    //--------------------------------------------------------------------------
    /// Divergence of convective term (PPE approach)
    ///
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    ///
    /// @return     reduced third order tensor in List <Eigen::MatrixXd> format for the divergence of conv. term (used only with a PPE approach).
    ///
    List < Eigen::MatrixXd > div_momentum(label NUmodes, label NPmodes);

    //--------------------------------------------------------------------------
    /// Laplacian of pressure term (PPE approach)
    ///
    /// @param[in]  NPmodes    The number of pressure modes.
    ///
    /// @return     reduced matrix in Eigen::MatrixXd format for the laplacian of pressure.
    ///
    Eigen::MatrixXd laplacian_pressure(label NPmodes);

    //--------------------------------------------------------------------------
    /// @brief      Term N° 1 given by the additional boundary condition using a PPE approach
    ///
    /// @param[in]  NUmodes    The number of velocity modes.
    /// @param[in]  NPmodes    The number of pressure modes.
    ///
    /// @return     reduced matrix in Eigen::MatrixXd format for the BC1 using a PPE approach.
    ///
    Eigen::MatrixXd pressure_BC1(label NPmodes, label NUmodes);

    //--------------------------------------------------------------------------
    /// @brief      Term N° 2 given by the additional boundary condition using a PPE approach
    ///
    /// @param[in]  NPmodes  The n pmodes
    /// @param[in]  NUmodes  The n umodes
    ///
    /// @return     reduced matrix in Eigen::MatrixXd format for the BC2 using a PPE approach.
    ///
    List < Eigen::MatrixXd > pressure_BC2(label NPmodes, label NUmodes);

    //--------------------------------------------------------------------------
    /// @brief      Term N° 3 given by the additional boundary condition using a PPE approach
    ///
    /// @param[in]  NPmodes  The n pmodes
    /// @param[in]  NUmodes  The n umodes
    ///
    /// @return     reduced third order tensor in List <Eigen::MatrixXd> format for the BC3 using a PPE approach.
    ///
    Eigen::MatrixXd pressure_BC3(label NPmodes, label NUmodes);

    //--------------------------------------------------------------------------
    /// Function to change the viscosity
    ///
    /// @param[in]  mu    viscosity (scalar)
    ///
    void change_viscosity(double mu);

    //--------------------------------------------------------------------------
    /// @brief      Compute lift and drag matrices
    ///
    /// @param[in]  NUmodes    The N of velocity modes
    /// @param[in]  NPmodes    The N of pressure modes
    /// @param[in]  NSUPmodes  The N of supremizer modes
    ///
    void Forces_matrices(label NUmodes, label NPmodes, label NSUPmodes);


};

#endif










