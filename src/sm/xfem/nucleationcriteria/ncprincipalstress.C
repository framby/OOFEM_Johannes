/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ncprincipalstress.h"
#include "error.h"
#include "xfem/enrichmentitem.h"
#include "domain.h"
#include "element.h"
#include "gausspoint.h"

#include "Materials/structuralms.h"
#include "Materials/structuralmaterial.h"

#include "xfem/enrichmentitems/crack.h"
#include "xfem/xfemmanager.h"
#include "xfem/enrichmentfunction.h"
#include "xfem/enrichmentfronts/enrichmentfrontlinbranchfunconeel.h"
#include "xfem/propagationlaws/plhoopstresscirc.h"
#include "dynamicdatareader.h"
#include "dynamicinputrecord.h"
#include "geometry.h"
#include "classfactory.h"

#include <memory>

namespace oofem {
REGISTER_NucleationCriterion(NCPrincipalStress)

NCPrincipalStress :: NCPrincipalStress(Domain *ipDomain) :
    NucleationCriterion(ipDomain),
    mStressThreshold(0.0),
    mInitialCrackLength(0.0),
    mCutOneEl(true)
{}

NCPrincipalStress :: ~NCPrincipalStress() {}

std :: vector < std :: unique_ptr < EnrichmentItem >> NCPrincipalStress :: nucleateEnrichmentItems()
{
    std :: vector < std :: unique_ptr < EnrichmentItem >> eiList;

    // Loop over all elements and all bulk GP.
    for ( auto &el : mpDomain->giveElements() ) {
        int numIR = el->giveNumberOfIntegrationRules();

        for ( int irInd = 0; irInd < numIR; irInd++ ) {
            IntegrationRule *ir = el->giveIntegrationRule(irInd);

            for ( auto &gp : *ir ) {
                StructuralMaterialStatus *ms = dynamic_cast< StructuralMaterialStatus * >( gp->giveMaterialStatus() );

                if ( ms != NULL ) {
                    const FloatArray &stress = ms->giveTempStressVector();

                    FloatArray principalVals;
                    FloatMatrix principalDirs;
                    StructuralMaterial :: computePrincipalValDir(principalVals, principalDirs, stress, principal_stress);

                    if ( principalVals [ 0 ] > mStressThreshold ) {
                        printf("\nFound GP with stress above threshold.\n");
                        //printf("principalVals: "); principalVals.printYourself();

                        FloatArray crackNormal;
                        crackNormal.beColumnOf(principalDirs, 1);
                        printf("crackNormal: ");
                        crackNormal.printYourself();

                        FloatArray crackTangent = {
                            -crackNormal(1), crackNormal(0)
                        };
                        crackTangent.normalize();
                        //printf("crackTangent: "); crackTangent.printYourself();



                        // Create geometry
                        FloatArray pc = {
                            gp->giveGlobalCoordinates() (0), gp->giveGlobalCoordinates() (1)
                        };
                        //printf("Global coord: "); pc.printYourself();


                        FloatArray ps = pc;
                        ps.add(-0.5 * mInitialCrackLength, crackTangent);

                        FloatArray pe = pc;
                        pe.add(0.5 * mInitialCrackLength, crackTangent);

                        if ( mCutOneEl ) {
                            // If desired, ensure that the crack cuts exactly one element.
                            Line line(ps, pe);
                            std :: vector< FloatArray >intersecPoints;
                            //line.computeIntersectionPoints(el.get(), intersecPoints);

#if 0
                            for ( int i = 1; i <= el->giveNumberOfDofManagers(); i++ ) {
                                int n1 = i;
                                int n2 = 0;
                                if ( i < el->giveNumberOfDofManagers() ) {
                                    n2 = i + 1;
                                } else {
                                    n2 = 1;
                                }

                                const FloatArray &p1 = *(el->giveDofManager(n1)->giveCoordinates());
                                const FloatArray &p2 = *(el->giveDofManager(n2)->giveCoordinates());
                            }
#endif

                            printf( "intersecPoints.size(): %lu\n", intersecPoints.size() );

                            if ( intersecPoints.size() == 2 ) {
                                ps = std :: move(intersecPoints [ 0 ]);
                                pe = std :: move(intersecPoints [ 1 ]);
                            } else {
                                OOFEM_ERROR("intersecPoints.size() != 2")
                            }
                        }

                        FloatArray points = {
                            ps(0), ps(1), pc(0), pc(1), pe(0), pe(1)
                        };

                        double diffX = 0.5 * ( ps(0) + pe(0) ) - pc(0);
                        printf("diffX: %e\n", diffX);

                        double diffY = 0.5 * ( ps(1) + pe(1) ) - pc(1);
                        printf("diffY: %e\n", diffY);

                        XfemManager *xMan = mpDomain->giveXfemManager();
                        int n = xMan->giveNumberOfEnrichmentItems() + 1;
                        std :: unique_ptr< Crack >crack( new Crack(n, xMan, mpDomain) );


                        // Geometry
                        std :: unique_ptr< BasicGeometry >geom = std :: unique_ptr< BasicGeometry >( new PolygonLine() );
                        geom->insertVertexBack(ps);
                        geom->insertVertexBack(pc);
                        geom->insertVertexBack(pe);
                        crack->setGeometry( std :: move(geom) );

                        // Enrichment function
                        EnrichmentFunction *ef = new HeavisideFunction(1, mpDomain);
                        crack->setEnrichmentFunction(ef);

                        // Enrichment fronts
                        EnrichmentFront *efStart = new EnrFrontLinearBranchFuncOneEl();
                        crack->setEnrichmentFrontStart(efStart);

                        EnrichmentFront *efEnd = new EnrFrontLinearBranchFuncOneEl();
                        crack->setEnrichmentFrontEnd(efEnd);


                        ///////////////////////////////////////
                        // Propagation law

                        // Options
#if 0
                        double radius = 0.5*mInitialCrackLength, angleInc = 10.0, incrementLength = 0.5*mInitialCrackLength, hoopStressThreshold = 0.0;
                        bool useRadialBasisFunc = true;

                        PLHoopStressCirc *pl = new PLHoopStressCirc();
                        pl->setRadius(radius);
                        pl->setAngleInc(angleInc);
                        pl->setIncrementLength(incrementLength);
                        pl->setHoopStressThreshold(hoopStressThreshold);
                        pl->setUseRadialBasisFunc(useRadialBasisFunc);
#endif

                        PLDoNothing *pl = new PLDoNothing();
                        crack->setPropagationLaw(pl);

                        eiList.push_back( std :: unique_ptr< EnrichmentItem >( std :: move(crack) ) );

                        // We only introduce one crack per element in a single time step.
                        break;
                    }
                }
            }
        }
    }


    return eiList;
}


IRResultType NCPrincipalStress :: initializeFrom(InputRecord *ir)
{
    IRResultType result; // Required by IR_GIVE_FIELD macro

    IR_GIVE_FIELD(ir, mStressThreshold, _IFT_NCPrincipalStress_StressThreshold);
    printf("mStressThreshold: %e\n", mStressThreshold);

    IR_GIVE_FIELD(ir, mInitialCrackLength, _IFT_NCPrincipalStress_InitialCrackLength);
    printf("mInitialCrackLength: %e\n", mInitialCrackLength);

    return NucleationCriterion :: initializeFrom(ir);
}

void NCPrincipalStress :: appendInputRecords(DynamicDataReader &oDR)
{
    DynamicInputRecord *ir = new DynamicInputRecord();

    ir->setRecordKeywordField(this->giveInputRecordName(), 1);

    ir->setField(mStressThreshold, _IFT_NCPrincipalStress_StressThreshold);
    ir->setField(mInitialCrackLength, _IFT_NCPrincipalStress_InitialCrackLength);

    oDR.insertInputRecord(DataReader :: IR_crackNucleationRec, ir);

    // Enrichment function
    DynamicInputRecord *efRec = new DynamicInputRecord();
    mpEnrichmentFunc->giveInputRecord(* efRec);
    oDR.insertInputRecord(DataReader :: IR_enrichFuncRec, efRec);
}
} /* namespace oofem */
