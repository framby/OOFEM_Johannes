/* $Header: /home/cvs/bp/oofem/oofemlib/src/primaryfield.h,v 1.1 2003/04/06 14:08:25 bp Exp $ */
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
 *               Copyright (C) 1993 - 2008   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef primaryfield_h
#define primaryfield_h

#include "field.h"
#include "interface.h"

#include "flotarry.h"
#include "equationid.h"
#include "valuemodetype.h"
#include "contextioresulttype.h"
#include "contextmode.h"

#ifndef __MAKEDEPEND
 #include <vector>
#endif

namespace oofem {
class PrimaryField;
class Dof;

/**
 * Element interface class. Declares the functionality required to support PrimaryField element interpolation.
 */
class EIPrimaryFieldInterface : public Interface
{
public:

    EIPrimaryFieldInterface() : Interface() { }
    /**
     * Evaluates the value of field at given point of interest (should be located inside receiver's volume) using
     * element interpolation.
     * @return returns zero if ok, nonzero when error encountered.
     */
  virtual int EIPrimaryFieldI_evaluateFieldVectorAt(FloatArray &answer, PrimaryField &pf,
						    FloatArray &coords, IntArray &dofId, ValueModeType mode,
						    TimeStep *atTime) = 0;
};


/**
 * Abstract class representing field of primary variables (those, which are unknown and are typically
 * associated to nodes).
 * In the current design the primary field is understood as simple database, that allows to keep track
 * of the history of a solution vector representing primary field. The histrory is kept as sequence of
 * solution vectors. The history depth kept can be selected. PrimaryField class basically provides access to
 * time-dependent vectors of the field of unknowns. It adds the posibility to
 * further interpolate the field values using element interpolation functions.
 * The prescribed values of the field are not maintatined, since they can be obtained directly from
 * corresponding DOFs of associated domain.
 *
 * As the PrimaryField stores the state directly in solution vectors that are usually directly 
 * updated by EngngModel, it may contain a mix of different fields (this is especially true for 
 * strongly coupled problems). Then masked primary field can be used to select only certain DOFs
 * (based on DofID) from its master PrimaryField.
 */
class PrimaryField : public Field
{
public:


protected:
    int actualStepNumber;
    int actualStepIndx;
    int nHistVectors;
    AList< FloatArray >solutionVectors;
    AList< TimeStep >solStepList;
    //IntArray atomicFields;
    EngngModel *emodel;
    int domainIndx;
    EquationID ut;
public:
    /** Constructor. Creates a field of given type associated to given domain.
     * Not using pointer to domain, because this will prevent the use of PrimaryField as an
     * EngngModel attribute. This is because the domain does not exists when
     * PrimaryField is created (this is when EngngModel is created).
     */
    PrimaryField(EngngModel *a, int idomain, FieldType ft, EquationID ut, int nHist);
    ~PrimaryField();
    /** Copy unknowns from previous solution or DOF's dictionary to the solution vector
     * @param mode what the unknown desribes (increment, total value etc.)
     * @param atTime time of interest
     * @param answer the resulting vector
     */
    virtual void initialize(ValueModeType mode, TimeStep *atTime, FloatArray &answer);

    /** Return value of interest at given DOF
     * @param dof pointer to DOF
     * @param mode what the unknown desribes (increment, total value etc.)
     * @param atTime time of interest
     */
    virtual double giveUnknownValue(Dof *dof, ValueModeType mode, TimeStep *atTime);
    /** Evaluates the field at given point
     * @param coords coordinates of the point of interest
     * @return error code (0-ok, 1-point not found in domain)
     */
    virtual int evaluateAt(FloatArray &answer, FloatArray &coords,
                           ValueModeType mode, TimeStep *atTime);
    /** Evaluates the field at given DofManager
     * @param dofMan reference to dofManager
     * @return error code (0-ok, 1-point not found in domain)
     */
    virtual int evaluateAt(FloatArray &answer, DofManager* dman,
                           ValueModeType mode, TimeStep *atTime);
    /** Evaluates the field at given dofman, allows to select specific 
     * dofs using mask.
     * @param coords coordinates of the point of interest
     * @param dofId dof mask, id set to NULL, all Dofs evaluated
     * return error code (0=ok, 1=point not found in domain)
     */
    virtual int __evaluateAt(FloatArray &answer, DofManager* dman,
			     ValueModeType mode, TimeStep *atTime, 
			     IntArray *dofId);
    /** Evaluates the field at given point, allows to select specific 
     * dofs using mask.
     * @param coords coordinates of the point of interest
     * @param dofId dof mask, id set to NULL, all Dofs evaluated
     * return error code (0=ok, 1=point not found in domain)
     */
    virtual int __evaluateAt(FloatArray &answer, FloatArray& coords,
			     ValueModeType mode, TimeStep *atTime, 
			     IntArray *dofId);
    /**
     */
    virtual FloatArray *giveSolutionVector(TimeStep *atTime);

    /** Project @param vectorToStore back to DOF's dictionary
     * @param mode what the unknown desribes (increment, total value etc.)
     * @param atTime time
     * @param vectorToStore vector with the size of number of equations
     */
    virtual void update(ValueModeType mode, TimeStep *atTime, FloatArray &vectorToStore)
    {  };

    /**
     */
    virtual void advanceSolution(TimeStep *atTime);

    /** Stores receiver state to output stream.
     * Writes the FEMComponent class-id in order to allow test whether correct data are then restored.
     * @param stream output stream
     * @param mode determines ammount of info required in stream (state, definition,...)
     * @return contextIOResultType
     * @exception throws an ContextIOERR exception if error encountered
     */
    virtual contextIOResultType    saveContext(DataStream *stream, ContextMode mode);
    /** Restores the receiver state previously written in stream.
     * Reads the FEMComponent class-id in order to allow test consistency.
     * @see saveContext member function.
     * @return contextIOResultType
     * @exception throws an ContextIOERR exception if error encountered
     */
    virtual contextIOResultType    restoreContext(DataStream *stream, ContextMode mode);

    EquationID giveEquationID() { return this->ut; }

protected:
    int resolveIndx(TimeStep *atTime, int shift);
    virtual FloatArray *giveSolutionVector(int);
};

} // end namespace oofem
#endif // primaryfield_h
