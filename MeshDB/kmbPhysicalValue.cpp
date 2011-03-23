/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : PhysicalValue                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbPhysicalValue.h"
#include <cmath>
#include <cfloat>

kmb::PhysicalValue::valueType
kmb::PhysicalValue::string2valueType(std::string str)
{
	if( str == "NONE" )					return kmb::PhysicalValue::NONE;
	else if( str == "BOOLEAN" )			return kmb::PhysicalValue::BOOLEAN;
	else if( str == "INTEGER" )			return kmb::PhysicalValue::INTEGER;
	else if( str == "SCALAR" )			return kmb::PhysicalValue::SCALAR;
	else if( str == "VECTOR2" )			return kmb::PhysicalValue::VECTOR2;
	else if( str == "VECTOR3" )			return kmb::PhysicalValue::VECTOR3;
	else if( str == "VECTOR4" )			return kmb::PhysicalValue::VECTOR4;
	else if( str == "POINT3VECTOR3" )	return kmb::PhysicalValue::POINT3VECTOR3;
	else if( str == "TENSOR6" )			return kmb::PhysicalValue::TENSOR6;
	else if( str == "VECTOR2WITHINT" )	return kmb::PhysicalValue::VECTOR2WITHINT;
	else if( str == "ARRAY" )			return kmb::PhysicalValue::ARRAY;
	else if( str == "HASH" )			return kmb::PhysicalValue::HASH;
	else if( str == "INTHASH" )			return kmb::PhysicalValue::INTHASH;
	else if( str == "STRING" )			return kmb::PhysicalValue::STRING;
	else if( str == "UNDEFINED" )		return kmb::PhysicalValue::UNDEFINED;
	else								return kmb::PhysicalValue::UNKNOWN;
}

std::string
kmb::PhysicalValue::valueType2string(kmb::PhysicalValue::valueType t)
{
	switch(t){
		case kmb::PhysicalValue::NONE:			return std::string("NONE");
		case kmb::PhysicalValue::BOOLEAN:		return std::string("BOOLEAN");
		case kmb::PhysicalValue::INTEGER:		return std::string("INTEGER");
		case kmb::PhysicalValue::SCALAR:		return std::string("SCALAR");
		case kmb::PhysicalValue::VECTOR2:		return std::string("VECTOR2");
		case kmb::PhysicalValue::VECTOR3:		return std::string("VECTOR3");
		case kmb::PhysicalValue::VECTOR4:		return std::string("VECTOR4");
		case kmb::PhysicalValue::POINT3VECTOR3:	return std::string("POINT3VECTOR3");
		case kmb::PhysicalValue::TENSOR6:		return std::string("TENSOR6");
		case kmb::PhysicalValue::VECTOR2WITHINT:return std::string("VECTOR2WITHINT");
		case kmb::PhysicalValue::ARRAY:			return std::string("ARRAY");
		case kmb::PhysicalValue::HASH:			return std::string("HASH");
		case kmb::PhysicalValue::INTHASH:		return std::string("INTHASH");
		case kmb::PhysicalValue::STRING:		return std::string("STRING");
		case kmb::PhysicalValue::UNDEFINED:		return std::string("UNDEFINED");
		default:								return std::string("UNKNOWN");
	}
}

int
kmb::PhysicalValue::getDimension(valueType t)
{
	switch(t){
		case kmb::PhysicalValue::NONE:			return 0;
		case kmb::PhysicalValue::BOOLEAN:		return 0;
		case kmb::PhysicalValue::INTEGER:		return 0;
		case kmb::PhysicalValue::SCALAR:		return 1;
		case kmb::PhysicalValue::VECTOR2:		return 2;
		case kmb::PhysicalValue::VECTOR3:		return 3;
		case kmb::PhysicalValue::VECTOR4:		return 4;
		case kmb::PhysicalValue::POINT3VECTOR3:	return 6;
		case kmb::PhysicalValue::TENSOR6:		return 6;
		case kmb::PhysicalValue::VECTOR2WITHINT:return 2;
		case kmb::PhysicalValue::ARRAY:			return 0;
		case kmb::PhysicalValue::HASH:			return 0;
		case kmb::PhysicalValue::INTHASH:		return 0;
		case kmb::PhysicalValue::STRING:		return 0;
		case kmb::PhysicalValue::UNDEFINED:		return 0;
		default:								return 0;
	}
}

double
kmb::PhysicalValue::distance(const PhysicalValue* other) const
{
	const double d = distanceSq( other );
	if( d >= 0.0 ){
		return sqrt( distanceSq( other ) );
	}else{
		return DBL_MAX;
	}
}

double
kmb::BooleanValue::distanceSq(const PhysicalValue* other) const
{
	if( other->getType() == kmb::PhysicalValue::BOOLEAN ){
		const kmb::BooleanValue* bVal = reinterpret_cast<const kmb::BooleanValue*>(other);
		if( this->getValue() == bVal->getValue() ){
			return 0.0;
		}else{
			return 1.0;
		}
	}else if( this->getType() == other->getType() ){
		if( this == other ){
			return 0.0;
		}else{
			return 1.0;
		}
	}else{
		return -1.0;
	}
}

double
kmb::IntegerValue::distanceSq(const PhysicalValue* other) const
{
	if( other->getType() == kmb::PhysicalValue::INTEGER ){
		const kmb::IntegerValue* iVal = reinterpret_cast<const kmb::IntegerValue*>(other);
		const int diff = this->value - iVal->getValue();
		return static_cast<double>(diff*diff);
	}else{
		return -1.0;
	}
}

double
kmb::ScalarValue::distanceSq(const PhysicalValue* other) const
{
	if( other->getType() == kmb::PhysicalValue::SCALAR ){
		const kmb::ScalarValue* sVal = reinterpret_cast<const kmb::ScalarValue*>(other);
		const double diff = this->value - sVal->getValue();
		return diff*diff;
	}else{
		return -1.0;
	}
}

double
kmb::Vector2Value::distanceSq(const PhysicalValue* other) const
{
	if( other->getType() == kmb::PhysicalValue::VECTOR2 ){
		const kmb::Vector2Value* vVal = reinterpret_cast<const kmb::Vector2Value*>(other);
		const double d0 = this->getValue(0) - vVal->getValue(0);
		const double d1 = this->getValue(1) - vVal->getValue(1);
		return d0*d0 + d1*d1;
	}else{
		return -1.0;
	}
}

double
kmb::Vector2Value::getLength(void) const
{
	return sqrt(value[0]*value[0] + value[1]*value[1]);
}

double
kmb::Vector3Value::distanceSq(const PhysicalValue* other) const
{
	if( other->getType() == kmb::PhysicalValue::VECTOR3 ){
		const kmb::Vector3Value* vVal = reinterpret_cast<const kmb::Vector3Value*>(other);
		const double d0 = this->getValue(0) - vVal->getValue(0);
		const double d1 = this->getValue(1) - vVal->getValue(1);
		const double d2 = this->getValue(2) - vVal->getValue(2);
		return d0*d0 + d1*d1 + d2*d2;
	}else{
		return -1.0;
	}
}

double
kmb::Vector3Value::getLength(void) const
{
	return sqrt(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]);
}

double
kmb::Vector4Value::distanceSq(const PhysicalValue* other) const
{
	if( other->getType() == kmb::PhysicalValue::VECTOR4 ){
		const kmb::Vector4Value* vVal = reinterpret_cast<const kmb::Vector4Value*>(other);
		const double d0 = this->getValue(0) - vVal->getValue(0);
		const double d1 = this->getValue(1) - vVal->getValue(1);
		const double d2 = this->getValue(2) - vVal->getValue(2);
		const double d3 = this->getValue(3) - vVal->getValue(3);
		return d0*d0 + d1*d1 + d2*d2 + d3*d3;
	}else{
		return -1.0;
	}
}

double
kmb::Vector4Value::getLength(void) const
{
	return sqrt(value[0]*value[0] + value[1]*value[1] + value[2]*value[2] + value[3]*value[3]);
}
