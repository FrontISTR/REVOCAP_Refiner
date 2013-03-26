/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : BLArray                                                 #
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

#include "Common/kmbBLArray.h"

unsigned int kmb::BLArrayBase::MAX_BITLENGTH = 20U;

size_t
kmb::BLArrayBase::getSize(void) const
{
	return topSize*subSize;
}

size_t
kmb::BLArrayBase::getTopSize(void) const
{
	return topSize;
}

size_t
kmb::BLArrayBase::getSubSize(void) const
{
	return subSize;
}

kmb::BLArrayIndex
kmb::BLArrayBase::getBLArrayIndex(size_t index) const
{
	kmb::BLArrayIndex bIndex;
	bIndex.subSize = this->subSize;
	bIndex.topIndex = index>>bitlength;
	bIndex.subIndex = index&localbitmask;
	return bIndex;
}

kmb::BLArrayIndex
kmb::BLArrayBase::getBLArrayIndex(size_t tIndex,size_t sIndex) const
{
	kmb::BLArrayIndex bIndex;
	bIndex.subSize = this->subSize;
	bIndex.topIndex = tIndex;
	bIndex.subIndex = sIndex;
	return bIndex;
}

bool
kmb::BLArrayBase::valid(const kmb::BLArrayIndex &index) const
{
	return (index.topIndex < this->topSize) && (index.subIndex < this->subSize);
}

kmb::BLArrayIndex::BLArrayIndex(void)
: topIndex(0U)
, subSize(0U)
, subIndex(0U)
{
}

kmb::BLArrayIndex::BLArrayIndex(const BLArrayIndex &other)
: topIndex(0U)
, subSize(0U)
, subIndex(0U)
{
	*this = other;
}

kmb::BLArrayIndex::~BLArrayIndex(void)
{
}

kmb::BLArrayIndex&
kmb::BLArrayIndex::operator=(const BLArrayIndex &other)
{
	this->topIndex = other.topIndex;
	this->subSize = other.subSize;
	this->subIndex = other.subIndex;
	return *this;
}

bool
kmb::BLArrayIndex::operator<(const BLArrayIndex &other) const
{
	return topIndex < other.topIndex || (topIndex==other.topIndex && subIndex<other.subIndex);
}

bool
kmb::BLArrayIndex::operator<=(const BLArrayIndex &other) const
{
	return topIndex <= other.topIndex || (topIndex==other.topIndex && subIndex<=other.subIndex);
}

void
kmb::BLArrayIndex::clear(void)
{
	this->topIndex = 0U;
	this->subIndex = 0U;
}

kmb::BLArrayIndex&
kmb::BLArrayIndex::operator++(void)
{
	subIndex++;
	if( subIndex == subSize ){
		topIndex++;
		subIndex = 0;
	}
	return *this;
}

kmb::BLArrayIndex&
kmb::BLArrayIndex::operator--(void)
{
	if( subIndex == 0 ){
		subIndex = subSize-1;
		topIndex--;
	}else{
		subIndex--;
	}
	return *this;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::BLArrayIndex
kmb::BLArrayIndex::operator++(int n)
{
	kmb::BLArrayIndex org = *this;
	++subIndex;
	if( subIndex == subSize ){
		++topIndex;
		subIndex = 0;
	}
	return org;
}

kmb::BLArrayIndex
kmb::BLArrayIndex::operator--(int n)
{
	kmb::BLArrayIndex org = *this;
	if( subIndex == 0 ){
		subIndex = subSize-1;
		topIndex--;
	}else{
		subIndex--;
	}
	return org;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

bool
kmb::BLArrayIndex::valid(void) const
{
	return (subIndex < subSize);
}

size_t
kmb::BLArrayIndex::getIndex(void) const
{
	return topIndex*subSize + subIndex;
}

size_t
kmb::BLArrayIndex::getSubIndex(void) const
{
	return subIndex;
}

size_t
kmb::BLArrayIndex::getTopIndex(void) const
{
	return topIndex;
}
