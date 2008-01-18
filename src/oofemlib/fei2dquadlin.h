/* $Header: /home/cvs/bp/oofem/oofemlib/src/fei2dquadlin.h,v 1.1 2003/04/06 14:08:24 bp Exp $ */
/*

                   *****    *****   ******  ******  ***   ***                            
                 **   **  **   **  **      **      ** *** **                             
                **   **  **   **  ****    ****    **  *  **                              
               **   **  **   **  **      **      **     **                               
              **   **  **   **  **      **      **     **                                
              *****    *****   **      ******  **     **         
            
                                                                   
               OOFEM : Object Oriented Finite Element Code                 
                    
                 Copyright (C) 1993 - 2002   Borek Patzak                                       



         Czech Technical University, Faculty of Civil Engineering,
     Department of Structural Mechanics, 166 29 Prague, Czech Republic
                                                                               
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                                                                              
*/

#ifndef fei2dquadlin_h

#include "flotarry.h"
#include "intarray.h"
#include "domain.h"
#include "feinterpol2d.h"
#include "fei1dlin.h"

/**
 Class representing a 2d isoparametric linear interpolation based on natural coordinates.
*/
class FEI2dQuadLin : public FEInterpolation2d
{

protected:
 int xind, yind;
 

public:
 FEI2dQuadLin (int ind1, int ind2) : FEInterpolation2d (1) {xind = ind1; yind=ind2;}

 /** 
  Evaluates the array of interpolation functions (shape functions) at given point.
  @param answer contains resulting array of evaluated interpolation functions
  @param lcoords array containing (local) coordinates 
  @param time time
  */
  virtual void evalN (FloatArray& answer, const FloatArray& lcoords, double time);
 /** 
  Evaluates the matrix of derivatives of interpolation functions (shape functions) at given point.
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param matrix contains resulting matrix of derivatives, the member at i,j position contains value of dNi/dxj
  @param nodes array of node numbers defining the interpolation geometry
  @param lcoords array containing (local) coordinates 
  @param time time
  */
  virtual void evaldNdx (FloatMatrix& answer, Domain* d, IntArray& nodes, const FloatArray& lcoords, double time) {
    const FloatArray* c[4];
    nodes2coords (d, nodes, c, 4);
    this->evaldNdx(answer, c, lcoords, time);
  }

 /** 
  Evaluates the matrix of derivatives of interpolation functions (shape functions) at given point.
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param matrix contains resulting matrix of derivatives, the member at i,j position contains value of dNi/dxj
  @param coords coordinates of nodes defining the interpolation geometry
  @param lcoords array containing (local) coordinates 
  @param time time
  */
  virtual void evaldNdx (FloatMatrix&answer, const FloatArray**  coords, const FloatArray& lcoords, double time);
 /** 
  Evaluates global coordinates from given local ones
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param answer contains resulting global coordinates
  @param nodes array of node numbers defining the interpolation geometry
  @param lcoords array containing (local) coordinates 
  @param time time
  */
  virtual void local2global (FloatArray& answer, Domain* d, IntArray& nodes, const FloatArray& lcoords, double time) {
    const FloatArray* c[4];
    nodes2coords (d, nodes, c, 4);
    this->local2global(answer, c, lcoords, time);
  }

 /** 
  Evaluates global coordinates from given local ones
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param answer contains resulting global coordinates
  @param coords coordinates of nodes defining the interpolation geometry
  @param lcoords array containing (local) coordinates 
  @param time time
  */
 virtual void local2global (FloatArray& answer, const FloatArray** coords, const FloatArray& lcoords, double time);
 /** 
  Evaluates local coordinates from given global ones. Returns nonzero if local coordinates are interpolating, 
  zero if extrapolating (nonzero is returned if point is within the element geometry, zero otherwise).
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param answer contains evaluated local coordinates
  @param nodes array of node numbers defining the interpolation geometry
  @param lcoords array containing (local) coordinates 
  @param time time
  @return nonzero is returned if point is within the element geometry, zero otherwise
  */
 virtual int  global2local (FloatArray& answer, Domain* d, IntArray& nodes, const FloatArray& lcoords, double time) {
   const FloatArray* c[4];
   nodes2coords (d, nodes, c, 4);
   return this->global2local(answer, c, lcoords, time);
 }

 /** 
  Evaluates local coordinates from given global ones. Returns nonzero if local coordinates are interpolating, 
  zero if extrapolating (nonzero is returned if point is within the element geometry, zero otherwise).
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param answer contains evaluated local coordinates
  @param coords coordinates of nodes defining the interpolation geometry
  @param lcoords array containing (local) coordinates 
  @param time time
  @return nonzero is returned if point is within the element geometry, zero otherwise
  */
 virtual int  global2local (FloatArray& answer, const FloatArray** coords, const FloatArray& lcoords, double time) ;
  /**
  Evaluates the jacobian of transformation between local and global coordinates.
  */
  virtual double giveTransformationJacobian (const FloatArray** coords, const FloatArray& lcoords, double time);
 /**
  Evaluates the jacobian of transformation between local and global coordinates.
  */
  virtual double giveTransformationJacobian (Domain *d, IntArray& nodes, const FloatArray& lcoords, double time) {
    const FloatArray *c[4];
    nodes2coords (d, nodes, c, 4);
    return this->giveTransformationJacobian(c, lcoords, time);    
  }



 /**@name Edge interpolation services */
 //@{
  virtual void computeLocalEdgeMapping (IntArray& edgeNodes, int iedge);
 /** 
  Evaluates the array of edge interpolation functions (shape functions) at given point.
  @param answer contains resulting array of evaluated interpolation functions
  @param lcoords array containing (local) coordinates 
  @param time time
  */
  virtual void edgeEvalN (FloatArray& answer, const FloatArray& lcoords, double time);
 /** 
  Evaluates the matrix of derivatives of edge interpolation functions (shape functions) at given point.
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param answer contains resulting matrix of derivatives, the member at i,j position contains value of dNi/dxj
  @param iedge determines the edge number
  @param nodes array of node numbers (for the whole element) defining the interpolation geometry
  @param lcoords array containing (local) coordinates 
  @param time time
  */
  virtual void edgeEvaldNdx (FloatMatrix&answer, int iedge, 
                             Domain* d, IntArray& nodes, const FloatArray& lcoords, double time) {
    const FloatArray *c[4];
    nodes2coords (d, nodes, c, 4);
    this->edgeEvaldNdx(answer, iedge, c, lcoords, time);
  }

 /** 
  Evaluates the matrix of derivatives of edge interpolation functions (shape functions) at given point.
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param answer contains resulting matrix of derivatives, the member at i,j position contains value of dNi/dxj
  @param iedge determines the edge number
  @param coords coordinates of nodes defining the interpolation geometry (for the whole element)
  @param lcoords array containing (local) coordinates 
  @param time time
  */
 virtual void edgeEvaldNdx (FloatMatrix&answer, int iedge, 
                            const FloatArray** coords, const FloatArray& lcoords, double time) ;
 /** 
  Evaluates edge global coordinates from given local ones
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param answer contains resulting global coordinates
  @param iedge determines edge number
  @param nodes array of node numbers (for the whole element) defining the interpolation geometry
  @param lcoords array containing (local) coordinates 
  @param time time
  */
  virtual void edgeLocal2global (FloatArray& answer, int iedge, 
                                 Domain* d, IntArray& nodes, const FloatArray& lcoords, double time) {
    const FloatArray* c[4];
    nodes2coords (d, nodes, c, 4);
    this->edgeLocal2global(answer, iedge, c, lcoords, time);
  }    
 /** 
  Evaluates edge global coordinates from given local ones
  These derivatives are in global coordinate system (where the nodal coordinates are defined) 
  @param answer contains resulting global coordinates
  @param iedge determines edge number
  @param coords coordinates of nodes defining the interpolation geometry (for the whole element) 
  @param lcoords array containing (local) coordinates 
  @param time time
  */
  virtual void edgeLocal2global (FloatArray& answer, int iedge,
                                 const FloatArray** coords, const FloatArray& lcoords, double time);
 /**
  Evaluates the edge jacobian of transformation between local and global coordinates.
  */
  virtual double edgeGiveTransformationJacobian (int iedge, const FloatArray **coords, const FloatArray& lcoords, 
                                                 double time);
 /**
  Evaluates the edge jacobian of transformation between local and global coordinates.
  */
  virtual double edgeGiveTransformationJacobian (int iedge, Domain* d, IntArray& nodes, const FloatArray& lcoords, 
						 double time) {
    const FloatArray* c[4];
    nodes2coords (d, nodes, c, 4);
    return this->edgeGiveTransformationJacobian(iedge, c, lcoords, time);
  }
  //@}

protected:
  double edgeComputeLength (IntArray& edgeNodes, const FloatArray **coords);
  void   giveDerivativeKsi (FloatArray& n, double eta);
  void   giveDerivativeEta (FloatArray& n, double ksi);
  void   giveJacobianMatrixAt (FloatMatrix& jacobianMatrix, const FloatArray** coords, const FloatArray& lcoords) ;
 
} ;




#define fei2dquadlin_h
#endif





