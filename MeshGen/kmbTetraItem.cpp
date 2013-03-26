/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : TetraItem                                               #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshGen/kmbTetraItem.h"
#include "MeshDB/kmbElementRelation.h"
#include <map>
#include <cassert>

kmb::TetraN::TetraN(nodeIdType n0,nodeIdType n1,nodeIdType n2,nodeIdType n3)
	: Tetrahedron(n0,n1,n2,n3), neighbors(NULL), face(0)
{
	neighbors = new kmb::TetraN*[4];
	neighbors[0] = NULL;
	neighbors[1] = NULL;
	neighbors[2] = NULL;
	neighbors[3] = NULL;
}

kmb::TetraN::~TetraN(void)
{
	if( neighbors ){
		delete[] neighbors;
	}
}

void
kmb::TetraN::setNeighbor(int index, kmb::TetraN* nei,int f)
{
	neighbors[index] = nei;
	face &= ~(0x0f<<(4*index));
	face |= (f+2)<<(4*index);
}

kmb::TetraN*
kmb::TetraN::getNeighbor(int index) const
{
	return neighbors[index];
}

int
kmb::TetraN::getNeighborFace(int index) const
{
	return (face>>(index*4) & 0x0f) - 2;
}

int
kmb::TetraN::getNeighborIndex(kmb::TetraN* nei) const
{
	int index = -1;
	for(int i=0;i<3;++i){
		if( this->neighbors[i] == nei ){
			index = i;
			break;
		}
	}
	return index;
}

kmb::TetraItem::TetraItem(kmb::nodeIdType n0,kmb::nodeIdType n1,kmb::nodeIdType n2,kmb::nodeIdType n3)
: kmb::Tetrahedron(n0,n1,n2,n3)
, previousTetra(NULL)
, nextTetra(NULL)
, face(0)
{
	neighbors[0] = NULL;
	neighbors[1] = NULL;
	neighbors[2] = NULL;
	neighbors[3] = NULL;
}

kmb::TetraItem::~TetraItem(void)
{
	if( previousTetra ){
		previousTetra->nextTetra = this->nextTetra;
	}
	if( nextTetra ){
		nextTetra->previousTetra = this->previousTetra;
	}
}

void kmb::TetraItem::detach(void)
{
	if( getPrevious() != NULL ){
		getPrevious()->setNext( getNext() );
	}
	if( getNext() != NULL ){
		getNext()->setPrevious( getPrevious() );
	}
	setPrevious( NULL );
	setNext( NULL );
}

void kmb::TetraItem::detach(kmb::TetraItem* &first,kmb::TetraItem* &last)
{
	if( this == first ){
		first = getNext();
	}
	if( this == last ){
		last = getPrevious();
	}
	if( getPrevious() != NULL ){
		getPrevious()->setNext( getNext() );
	}
	if( getNext() != NULL ){
		getNext()->setPrevious( getPrevious() );
	}
	setPrevious( NULL );
	setNext( NULL );
}

void kmb::TetraItem::attach(kmb::TetraItem* last)
{
	if( last != NULL ){
		last->setNext( this );
		this->setPrevious( last );
	}
}

void kmb::TetraItem::attach(kmb::TetraItem* &first,kmb::TetraItem* &last)
{
	if( first == NULL || last == NULL ){
		first = this;
		last = this;
	}else{
		last->setNext( this );
		this->setPrevious( last );
		last = this;
	}
}


void
kmb::TetraItem::setNeighbor(int index, kmb::TetraItem* nei,int f)
{
	neighbors[index] = nei;
	face &= ~(0x0f<<(4*index));
	face |= (f+2)<<(4*index);
}

kmb::TetraItem*
kmb::TetraItem::getNeighbor(int index) const
{
	return neighbors[index];
}

int
kmb::TetraItem::getNeighborFace(int index) const
{
	return (face>>(index*4) & 0x0f) - 2;
}

int
kmb::TetraItem::getNeighborIndex(kmb::TetraItem* nei) const
{
	int index = -1;
	for(int i=0;i<3;++i){
		if( this->neighbors[i] == nei ){
			index = i;
			break;
		}
	}
	return index;
}

void
kmb::TetraItem::setAdjacentPair(kmb::TetraItem* t0,int i0,kmb::TetraItem* t1,int i1)
{
	if( t0 && t1 ){
		t0->setNeighbor(i0,t1,i1);
		t1->setNeighbor(i1,t0,i0);
	}
}

int
kmb::TetraItem::deleteItems( kmb::TetraItem* first )
{
	if( first == NULL ){
		return 0;
	}
	int count = 0;
	kmb::TetraItem* tetra = first;
	kmb::TetraItem* nextTetra = NULL;
	while( tetra ){
		nextTetra = tetra->getNext();
		for(int i=0;i<4;++i){
			if( tetra->getNeighbor(i) ){
				int index = tetra->getNeighborFace(i);
				if(index >= 0){
					tetra->getNeighbor(i)->setNeighbor(index,NULL,-2);
				}
				tetra->setNeighbor(i,NULL,-2);
			}
		}
		delete tetra;
		tetra = nextTetra;
		++count;
	}
	return count;
}

#include <iostream>

void kmb::TetraItem::generateNeighborRelations( kmb::TetraItem* tetra )
{
	typedef std::pair<kmb::TetraItem*,int> FaceItem;
	std::multimap< int, FaceItem > facemap;
	kmb::TetraItem* tIter = tetra;
	while( tIter ){
		for(int i=0;i<4;++i){
			int n = tIter->getBoundaryCellId(i,0) + tIter->getBoundaryCellId(i,1) + tIter->getBoundaryCellId(i,2);
			FaceItem f(tIter,i);
			facemap.insert( std::pair<int, FaceItem>(n,f) );
		}
		tIter = tIter->getNext();
	}
	std::multimap< int, FaceItem >::iterator fIter = facemap.begin();
	while( fIter != facemap.end() ){
		int n0 = fIter->first;
		kmb::TetraItem* t0 = fIter->second.first;
		int b0 = fIter->second.second;
		std::multimap< int, FaceItem >::iterator lIter = facemap.lower_bound(n0);
		std::multimap< int, FaceItem >::iterator uIter = facemap.upper_bound(n0);

		while( lIter != uIter ){
			kmb::TetraItem* t1 = lIter->second.first;
			int b1 = lIter->second.second;
			int i0(-1),i1(-1);
			if( kmb::ElementRelation::getRelation( *t0, i0, *t1, i1 ) == kmb::ElementRelation::ADJACENT ){
				if( i0==b0 && t0->getNeighbor(i0)==NULL && i1==b1 && t1->getNeighbor(i1)==NULL ){
					t0->setNeighbor(b0,t1,b1);
					t1->setNeighbor(b1,t0,b0);
				}
			}
			++lIter;
		}
		++fIter;
	}
	tIter = tetra;
	bool flag = true;
	std::cout << "kmb::TetraItem::generateNeighborRelations begin" << std::endl;
	while( tIter ){

		kmb::TetraItem* t0 = tIter->getNeighbor(0);
		kmb::TetraItem* t1 = tIter->getNeighbor(1);
		kmb::TetraItem* t2 = tIter->getNeighbor(2);
		kmb::TetraItem* t3 = tIter->getNeighbor(3);
		flag &= ( t0 != NULL );
		flag &= ( t1 != NULL );
		flag &= ( t2 != NULL );
		flag &= ( t3 != NULL );
		int i0,i1;
		kmb::ElementRelation::relationType rel = kmb::ElementRelation::getRelation( *tIter, i0, *t0, i1);
		flag &= ( rel == kmb::ElementRelation::ADJACENT );
		flag &= ( i0 == 0 );
		flag &= ( i1 == tIter->getNeighborFace(i0) );
		flag &= ( i0 == t0->getNeighborFace(i1) );
		assert( flag );
		tIter = tIter->getNext();
	}
	std::cout << "kmb::TetraItem::generateNeighborRelations end" << std::endl;
}

#include <iostream>
#include "MeshDB/kmbElementRelation.h"

int
kmb::TetraItem::debugItem( kmb::TetraItem* item, const char* tag  )
{
	if( item == NULL ){
		std::cout << "NULL" << std::endl;
		return 0;
	}
	if( tag ){
		std::cout << tag << " ";
	}
	std::cout << "[ "
		<< item->getCellId(0) << " "
		<< item->getCellId(1) << " "
		<< item->getCellId(2) << " "
		<< item->getCellId(3) << "] => ";
	for(int i=0;i<4;++i){
		if( item->getNeighbor(i) ){
			std::cout << "[ "
				<< item->getNeighbor(i)->getCellId(0) << " "
				<< item->getNeighbor(i)->getCellId(1) << " "
				<< item->getNeighbor(i)->getCellId(2) << " "
				<< item->getNeighbor(i)->getCellId(3) << "] ";
		}else{
			std::cout << "[NULL] ";
		}
	}
	std::cout << std::endl;
	return 1;
}

int
kmb::TetraItem::debugItems( kmb::TetraItem* first, const char* tag )
{
	if( first == NULL ){
		std::cout << "Debug Items NULL" << std::endl;
		return 0;
	}
	if( tag ){
		std::cout << "Debug Items " << tag << std::endl;
	}else{
		std::cout << "Debug Items" << std::endl;
	}
	int ret = 0;
	kmb::TetraItem* tetra = first;
	while( tetra ){
		ret += kmb::TetraItem::debugItem( tetra );
		tetra = tetra->nextTetra;
	}
	if( tag ){
		std::cout << "Debug Items End " << tag << std::endl;
	}else{
		std::cout << "Debug Items End" << std::endl;
	}
	return ret;
}

bool
kmb::TetraItem::checkVolume( kmb::TetraItem* tetra, kmb::Point3DContainer &points )
{
	if( tetra == NULL ){
		std::cout << "Check Volume NULL" << std::endl;
		return false;
	}
	bool retVal = true;
	std::cout << "[ "
		<< tetra->getCellId(0) << " "
		<< tetra->getCellId(1) << " "
		<< tetra->getCellId(2) << " "
		<< tetra->getCellId(3) << "] => ";
	kmb::Point3D p0,p1,p2,p3;
	if( points.getPoint((*tetra)[0],p0) &&
		points.getPoint((*tetra)[1],p1) &&
		points.getPoint((*tetra)[2],p2) &&
		points.getPoint((*tetra)[3],p3) ){
		double vol = kmb::Point3D::volume( p0, p1, p2, p3 );
		std::cout << vol;
		if( vol < 0.0 ){
			retVal = false;
		}
	}else{
		std::cout << "null";
		retVal = false;
	}
	std::cout << std::endl;
	return retVal;
}

bool
kmb::TetraItem::checkVolumes( kmb::TetraItem* tetra, kmb::Point3DContainer &points, const char* tag )
{
	if( tetra == NULL ){
		std::cout << "Check Volume NULL" << std::endl;
		return false;
	}
	bool retVal = true;
	while( tetra ){
		retVal &= checkVolume( tetra, points );
		tetra = tetra->getNext();
	}
	return retVal;
}

bool
kmb::TetraItem::checkNeighbor( kmb::TetraItem* tetra )
{
	if( tetra == NULL ){
		std::cout << "Check Neighbors NULL" << std::endl;
		return false;
	}
	bool retVal(true);
	std::cout << "[ "
		<< tetra->getCellId(0) << " "
		<< tetra->getCellId(1) << " "
		<< tetra->getCellId(2) << " "
		<< tetra->getCellId(3) << "] => ";
	for(int i=0;i<4;++i){

		kmb::TetraItem* neighbor = tetra->getNeighbor(i);
		int nFace = tetra->getNeighborFace(i);
		if( neighbor ){
			std::cout << "[ "
				<< neighbor->getCellId(0) << " "
				<< neighbor->getCellId(1) << " "
				<< neighbor->getCellId(2) << " "
				<< neighbor->getCellId(3) << "]"
				<< "(" << nFace << ") ";
			int i0=0, i1=0;
			assert( neighbor->getNeighbor(nFace) == tetra );
			if( kmb::ElementRelation::getRelation( *tetra, i0, *neighbor, i1 ) != kmb::ElementRelation::ADJACENT ){
				std::cout << "wrong relation! " << std::endl;
				debugItem( tetra );
				debugItem( neighbor );
				assert( false );
				retVal = false;
			}
			if( i0 != i || i1 != nFace ){
				std::cout << "wrong relation indices! " << std::endl;
				debugItem( tetra );
				debugItem( neighbor );
				assert( false );
				retVal = false;
			}
		}else{
			std::cout << "[NULL]() ";
		}
	}
	std::cout << std::endl;
	return retVal;
}

bool
kmb::TetraItem::checkNeighbors( kmb::TetraItem* tetra, const char* tag )
{
	if( tetra == NULL ){
		std::cout << "Check Neighbors NULL" << std::endl;
		return false;
	}
	if( tag ){
		std::cout << "Check Neighbors " << tag << std::endl;
	}
	bool retVal = true;
	while( tetra ){
		retVal &= checkNeighbor( tetra );
		tetra = tetra->getNext();
	}
	return retVal;
}
