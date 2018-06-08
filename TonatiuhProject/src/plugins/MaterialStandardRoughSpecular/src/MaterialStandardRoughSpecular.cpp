/***************************************************************************
Copyright (C) 2008 by the Tonatiuh Software Development Team.

This file is part of Tonatiuh.

Tonatiuh program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


Acknowledgments:

The development of Tonatiuh was started on 2004 by Dr. Manuel J. Blanco,
then Chair of the Department of Engineering of the University of Texas at
Brownsville. From May 2004 to July 2008, it was supported by the Department
of Energy (DOE) and the National Renewable Energy Laboratory (NREL) under
the Minority Research Associate (MURA) Program Subcontract ACQ-4-33623-06.
During 2007, NREL also contributed to the validation of Tonatiuh under the
framework of the Memorandum of Understanding signed with the Spanish
National Renewable Energy Centre (CENER) on February, 20, 2007 (MOU#NREL-07-117).
Since June 2006, the development of Tonatiuh is being led by the CENER, under the
direction of Dr. Blanco, now Director of CENER Solar Thermal Energy Department.

Developers: Manuel J. Blanco (mblanco@cener.com), Amaia Mutuberria, Victor Martin.

Contributors: Javier Garcia-Barberena, I�aki Perez, Inigo Pagola,  Gilda Jimenez,
Juana Amieva, Azael Mancillas, Cesar Cantu.
***************************************************************************/

#include "Ray.h"
#include "DifferentialGeometry.h"
#include "MaterialStandardRoughSpecular.h"
#include "NormalVector.h"
#include "RandomDeviate.h"
#include "sf.h"
#include "Transform.h"
#include "EnumeratedTypes.h"

TNodeType MaterialStandardRoughSpecular::m_nodeType = TNodeType::CreateEmptyType();

/*!
 * Creates a new instance of the class type corresponding object.
 */
std::shared_ptr< TNode > MaterialStandardRoughSpecular::CreateInstance( )
{
	//shared_prt needs a public constructor
	struct EnableCreateMaterialStandardRoughSpecular : public MaterialStandardRoughSpecular { using MaterialStandardRoughSpecular::MaterialStandardRoughSpecular; };
	return std::make_shared<EnableCreateMaterialStandardRoughSpecular>();
}

/*!
 * Initializes MaterialStandardRoughSpecular type.
 */
void MaterialStandardRoughSpecular::Init()
{

	MaterialStandardRoughSpecular::m_nodeType = TNodeType::CreateType( TNodeType::FromName( "Material" ), "MaterialStandardRoughSpecular", &MaterialStandardRoughSpecular::CreateInstance );
}

/*!
 * Creates a material object
 */
MaterialStandardRoughSpecular::MaterialStandardRoughSpecular()
:TMaterial(),
 m_reflectivityLabel( "reflectivity" ),
 m_sigmaSlopeLabel( "sigmaSlope" ),
 m_sigmaSpecularityLabel( "sigmaSpecularity" ),
 m_distributionLabel( "distribution" ),
 m_colorLabel( "color" ),
 m_transparencyLabel( "transparency" )
{
	SetName(GetType().GetName() );

	//Translation
	m_pParametersList->Append<double>( m_reflectivityLabel, 0.0, true );
	m_pParametersList->Append<double>( m_sigmaSlopeLabel, 2.0, true );
	m_pParametersList->Append<double>( m_sigmaSpecularityLabel, 0.0, true );

	EnumeratedTypes distributionEnumerator;
	distributionEnumerator.AddValue( "NORMAL", true );
	m_pParametersList->Append<EnumeratedTypes>( m_distributionLabel, distributionEnumerator, true );

	m_pParametersList->Append<std::string>(m_colorLabel,  "0.2 0.2 0.2", true );
	m_pParametersList->Append<double>( m_transparencyLabel, 0.0, true );
}


/*!
 * Destructor.
 */
MaterialStandardRoughSpecular::~MaterialStandardRoughSpecular()
{

}

/*!
 * Creates a copy of material node.
 */
std::shared_ptr< TNode > MaterialStandardRoughSpecular::Copy() const
{
	//shared_prt needs a public constructor
	struct EnableMaterialStandardRoughSpecular : public MaterialStandardRoughSpecular { using MaterialStandardRoughSpecular::MaterialStandardRoughSpecular; };
	std::shared_ptr<MaterialStandardRoughSpecular> materialNode = std::make_unique<EnableMaterialStandardRoughSpecular>();
	if( materialNode == 0 )	return ( 0  );

	 //Coping node parts.
	 //NO parts

	 //Coping the parameters.
	 materialNode->m_pParametersList->SetValue( m_reflectivityLabel, GetParameterValue<double>( m_reflectivityLabel ) );
	 materialNode->m_pParametersList->SetValue( m_sigmaSlopeLabel, GetParameterValue<double>( m_sigmaSlopeLabel ) );
	 materialNode->m_pParametersList->SetValue( m_sigmaSpecularityLabel, GetParameterValue<double>( m_sigmaSpecularityLabel ) );
	 materialNode->m_pParametersList->SetValue( m_distributionLabel, GetParameterValue<EnumeratedTypes>( m_distributionLabel ) );
	 materialNode->m_pParametersList->SetValue( m_colorLabel, GetParameterValue<std::string>( m_colorLabel ) );
	 materialNode->m_pParametersList->SetValue( m_transparencyLabel, GetParameterValue<double>( m_transparencyLabel ) );

	 return ( materialNode );
 }

/*!
 * Returns the material icon filename.
 */
std::string MaterialStandardRoughSpecular::GetIcon()
{
	return ( ":icons/MaterialStandardRoughSpecular.png" );
}

/*!
 * Returns material type.
 */
TNodeType MaterialStandardRoughSpecular::GetType() const
{
	return ( m_nodeType );
}

/*!
 * Checks if the material generates an \a outputRay for givem \a incident ray and intersection properties defined in \a dg.
 */
bool MaterialStandardRoughSpecular::OutputRay( const Ray& incident, DifferentialGeometry* dg, RandomDeviate& rand, Ray* outputRay ) const
{

	double reflectivity = GetParameterValue<double>( m_reflectivityLabel );
	double sigmaSlopeMRAD = GetParameterValue<double>( m_sigmaSlopeLabel );
	double sigmaSpecularityMRAD = GetParameterValue<double>( m_sigmaSpecularityLabel );

	double randomNumber = rand.RandomDouble();
	if ( randomNumber >= reflectivity  ) return false;

	//Compute reflected ray (local coordinates )
	outputRay->origin = dg->point;

	NormalVector normalVector;

	double sigmaNormal = sigmaSlopeMRAD / 1000;
	if( sigmaNormal > 0.0 )
	{
		NormalVector errorNormal = Normalize( NormalVector( ComputeErrorVector( sigmaNormal, rand ) ) );
		Vector3D r = dg->normal;
		Vector3D s = Normalize( dg->dpdu );
		Vector3D t = Normalize( dg->dpdv );
		Transform trasform( s.x, s.y, s.z, 0.0,
							r.x, r.y, r.z, 0.0,
							t.x, t.y, t.z, 0.0,
							0.0, 0.0, 0.0, 1.0);

		normalVector = trasform.GetInverse()( errorNormal );
	}
	else
	{
		normalVector = dg->normal;
	}


	double cosTheta = DotProduct( normalVector, incident.direction() );

	outputRay->setDirection( Normalize( incident.direction() - 2.0 * normalVector * cosTheta ) );



	//Add error to reflected ray
	double sigmaReflected= sigmaSpecularityMRAD / 1000;
	if( sigmaReflected > 0.0 )
	{
		Vector3D errorReflectedRay = ComputeErrorVector( sigmaReflected, rand );

		Vector3D r = outputRay->direction();
		Vector3D s = Normalize( CrossProduct( outputRay->direction(), dg->normal ) );
		Vector3D t = Normalize( CrossProduct( outputRay->direction(), s ) );
		Transform trasform( s.x, s.y, s.z, 0.0,
								r.x, r.y, r.z, 0.0,
								t.x, t.y, t.z, 0.0,
								0.0, 0.0, 0.0, 1.0);

		Vector3D errorReflectedRayDirection = trasform.GetInverse()( errorReflectedRay );
		outputRay->setDirection( errorReflectedRayDirection );
	}

	return true;
}

/*!
 * Computes a vector for defined distribution function and for the parameter  of the function defined.
 */
Vector3D MaterialStandardRoughSpecular::ComputeErrorVector( double simgaError, RandomDeviate& rand ) const
{
	EnumeratedTypes typeOfDistribution = GetParameterValue<EnumeratedTypes>( m_distributionLabel );
	std::string typeOfDistributionValue = typeOfDistribution.GetSelectedName();

	Vector3D errorVector;

	if( typeOfDistributionValue == "PILLBOX" )
	{
		//double phi = gc::TwoPi * rand.RandomDouble();
		//double theta = simgaError * rand.RandomDouble();

		//errorVector.x = sin( theta ) * sin( phi ) ;
		//errorVector.y = cos( theta );
		//errorVector.z = sin( theta ) * cos( phi );

	}
	else if( typeOfDistributionValue == "NORMAL" )
	{
		errorVector.x = simgaError * sf::AlternateBoxMuller( rand );
		errorVector.y = 1.0;
		errorVector.z = simgaError * sf::AlternateBoxMuller( rand );
	 }
	return errorVector;
}
