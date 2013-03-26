/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : PhysicalValue                                           #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
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
#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>
#include <cfloat>

#include "MeshDB/kmbTypes.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

namespace kmb{

class PhysicalValue
{
public:
	enum valueType{
		None,
		Boolean,
		Integer,
		Scalar,
		Vector2,
		Vector3,
		Vector4,
		Point3Vector3,
		Tensor6,
		NodeId,
		ElementId,
		Vector2withInt,
		Vector3withInt,
		Vector4withInt,
		Array,
		Hash,
		String,
		IntHash,
		Undefined,
		Unknown
	};
	static valueType string2valueType(std::string str);
	static std::string valueType2string(valueType t);
	static int getDimension(valueType t);
public:
	PhysicalValue(void){}
	virtual ~PhysicalValue(void){}
	virtual valueType getType(void) const = 0;
	virtual double distance(const PhysicalValue* other) const;
	virtual double distanceSq(const PhysicalValue* other) const{
		if(this == other){
			return 0.0;
		}else{
			return 1.0;
		}
	}
	virtual kmb::PhysicalValue* clone(void) = 0;
	virtual bool getValue(double* v) const { return false; }
	virtual bool getValue(long* k) const { return false; }
	virtual bool setValue(double* v){ return false; }
	virtual bool setValue(long* v){ return false; }
	virtual bool setValue(double v,int i=0){ return false; }
	virtual bool setValue(long k,int i=0){ return false; }
	virtual int getDimension(void) const { return 0; };
};

class BooleanValue : public PhysicalValue
{
private:
	bool value;
public:
	static const valueType vType = kmb::PhysicalValue::Boolean;
	BooleanValue(void) : value(false){}
	BooleanValue(bool v) : value(v){}
	virtual ~BooleanValue(void){}
	bool getValue(void) const { return value; }
	bool setValue(bool v){ this->value = v; return true; }
	virtual valueType getType(void) const { return Boolean; }
	virtual double distanceSq(const PhysicalValue* other) const;
	virtual kmb::PhysicalValue* clone(void){
		return new BooleanValue( this->value );
	}
};

class IntegerValue : public PhysicalValue
{
private:
	int value;
public:
	static const valueType vType = kmb::PhysicalValue::Integer;
	IntegerValue(void) : value(0){}
	IntegerValue(int v) : value(v){}
	virtual ~IntegerValue(void){}
	int getValue(void) const { return value; }
	bool setIValue(int v){ this->value = v; return true; }
	virtual valueType getType(void) const { return Integer; }
	virtual double distanceSq(const PhysicalValue* other) const;
	virtual kmb::PhysicalValue* clone(void){
		return new IntegerValue( this->value );
	}
	virtual bool getValue(long* k) const{
		k[0] = static_cast<long>(value);
		return true;
	}
	virtual bool setValue(long* v){
		value = static_cast<int>(*v);
		return true;
	}
	virtual bool setValue(long k,int i=0){
		value = static_cast<int>(k);
		return true;
	}
};

class ScalarValue : public PhysicalValue
{
private:
	double value;
public:
	static const valueType vType = kmb::PhysicalValue::Scalar;
	ScalarValue(void) : value(0.0){}
	ScalarValue(double v) : value(v){}
	ScalarValue(double* v) : value(*v){}
	virtual ~ScalarValue(void){}
	double getValue(void) const { return value; }
	bool setValue(double v,int i=0){ this->value = v; return true; }
	virtual valueType getType(void) const { return Scalar; }
	virtual double distanceSq(const PhysicalValue* other) const;
	virtual kmb::PhysicalValue* clone(void){
		return new ScalarValue( this->value );
	}
	virtual bool getValue(double* v) const {
		*v = value;
		return true;
	}
	bool setValue(double* v){ this->value = *v; return true; }
	int getDimension(void) const { return 1; };
};

class Vector2Value : public PhysicalValue
{
private:
	double value[2];
public:
	static const valueType vType = kmb::PhysicalValue::Vector2;
	Vector2Value(void){
		for(int i=0;i<2;++i){
			this->value[i] = 0.0;
		}
	}
	Vector2Value(double v[2]){
		for(int i=0;i<2;++i){
			this->value[i] = v[i];
		}
	}
	Vector2Value(double u,double v){
		this->value[0] = u;
		this->value[1] = v;
	}
	virtual ~Vector2Value(void){}
	double getValue(int i) const{ return value[i]; }
	double getLength(void) const;
	bool setValue(double v,int i){ this->value[i] = v; return true; }
	virtual valueType getType(void) const { return Vector2; }
	virtual double distanceSq(const PhysicalValue* other) const;
	virtual kmb::PhysicalValue* clone(void){
		return new Vector2Value( this->value );
	}
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		return true;
	}
	bool setValue(double* v){
		this->value[0] = v[0];
		this->value[1] = v[1];
		return true;
	}
	int getDimension(void) const { return 2; };
};

class Vector3Value : public PhysicalValue
{
private:
	double value[3];
public:
	static const valueType vType = kmb::PhysicalValue::Vector3;
	Vector3Value(void){
		for(int i=0;i<3;++i){
			this->value[i] = 0.0;
		}
	}
	Vector3Value(double v[3]){
		for(int i=0;i<3;++i){
			this->value[i] = v[i];
		}
	}
	Vector3Value(double u,double v,double w){
		this->value[0] = u;
		this->value[1] = v;
		this->value[2] = w;
	}
	virtual ~Vector3Value(void){}
	double getValue(int i) const{ return value[i]; }
	double getLength(void) const;
	bool setValue(double v,int i){ this->value[i] = v; return true; }
	virtual valueType getType(void) const { return Vector3; }
	virtual double distanceSq(const PhysicalValue* other) const;
	virtual kmb::PhysicalValue* clone(void){
		return new Vector3Value( this->value );
	}
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		v[2] = value[2];
		return true;
	}
	bool setValue(double* v){
		this->value[0] = v[0];
		this->value[1] = v[1];
		this->value[2] = v[2];
		return true;
	}
	int getDimension(void) const { return 3; };
};

class Vector4Value : public PhysicalValue
{
private:
	double value[4];
public:
	static const valueType vType = kmb::PhysicalValue::Vector4;
	Vector4Value(void){
		for(int i=0;i<4;++i){
			this->value[i] = 0.0;
		}
	}
	Vector4Value(double v[4]){
		for(int i=0;i<4;++i){
			this->value[i] = v[i];
		}
	}
	Vector4Value(double u,double v,double w,double z){
		this->value[0] = u;
		this->value[1] = v;
		this->value[2] = w;
		this->value[3] = z;
	}
	virtual ~Vector4Value(void){}
	double getValue(int i) const{ return value[i]; }
	double getLength(void) const;
	bool setValue(double v,int i){ this->value[i] = v; return true; }
	virtual valueType getType(void) const { return Vector4; }
	virtual double distanceSq(const PhysicalValue* other) const;
	virtual kmb::PhysicalValue* clone(void){
		return new Vector4Value( this->value );
	}
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		v[2] = value[2];
		v[3] = value[3];
		return true;
	}
	bool setValue(double* v){
		this->value[0] = v[0];
		this->value[1] = v[1];
		this->value[2] = v[2];
		this->value[3] = v[3];
		return true;
	}
	int getDimension(void) const { return 4; };
};

class Vector2WithInt : public PhysicalValue
{
private:
	double value[2];
	long ival;
public:
	static const valueType vType = kmb::PhysicalValue::Vector2withInt;
	Vector2WithInt(void){
		this->value[0] = 0.0;
		this->value[1] = 0.0;
		this->ival = 0;
	}
	Vector2WithInt(double u, double v,long i){
		this->value[0] = u;
		this->value[1] = v;
		this->ival = i;
	}
	virtual ~Vector2WithInt(void){}
	double getValue(int i) const{ return value[i]; }
	long getIValue(void) const{ return ival; };
	void setValue(double u,double v){ this->value[0] = u; this->value[1] = v; }
	bool setValue(double u,int i){ this->value[i] = u; return true; }
	void setIValue(long id){ this->ival = ival; }
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		return true;
	}
	bool setValue(double* v){
		this->value[0] = v[0];
		this->value[1] = v[1];
		return true;
	}
	virtual bool getValue(long* k) const {
		*k = ival;
		return true;
	}
	virtual valueType getType(void) const { return Vector2withInt; }
	virtual kmb::PhysicalValue* clone(void){
		return new Vector2WithInt( this->value[0], this->value[1], this->ival );
	}
	int getDimension(void) const { return 2; };
};

class Vector3WithInt : public PhysicalValue
{
private:
	double value[3];
	long ival;
public:
	static const valueType vType = kmb::PhysicalValue::Vector3withInt;
	Vector3WithInt(void){
		this->value[0] = 0.0;
		this->value[1] = 0.0;
		this->value[2] = 0.0;
		this->ival = 0;
	}
	Vector3WithInt(double u, double v, double w, long i){
		this->value[0] = u;
		this->value[1] = v;
		this->value[2] = w;
		this->ival = i;
	}
	virtual ~Vector3WithInt(void){}
	double getValue(int i) const{ return value[i]; }
	long getIValue(void) const{ return ival; };
	void setValue(double u,double v,double w){ this->value[0] = u; this->value[1] = v; this->value[2] = w; }
	bool setValue(double u,int i){ this->value[i] = u; return true; }
	void setIValue(long id){ this->ival = ival; }
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		v[2] = value[2];
		return true;
	}
	bool setValue(double* v){
		this->value[0] = v[0];
		this->value[1] = v[1];
		this->value[2] = v[2];
		return true;
	}
	virtual bool getValue(long* k) const {
		*k = ival;
		return true;
	}
	virtual valueType getType(void) const { return Vector3withInt; }
	virtual kmb::PhysicalValue* clone(void){
		return new Vector3WithInt( this->value[0], this->value[1], this->value[2], this->ival );
	}
	int getDimension(void) const { return 3; };
};

class Vector4WithInt : public PhysicalValue
{
private:
	double value[4];
	long ival;
public:
	static const valueType vType = kmb::PhysicalValue::Vector4withInt;
	Vector4WithInt(void){
		this->value[0] = 0.0;
		this->value[1] = 0.0;
		this->value[2] = 0.0;
		this->value[3] = 0.0;
		this->ival = 0;
	}
	Vector4WithInt(double u, double v, double w, double z, long i){
		this->value[0] = u;
		this->value[1] = v;
		this->value[2] = w;
		this->value[3] = z;
		this->ival = i;
	}
	virtual ~Vector4WithInt(void){}
	double getValue(int i) const{ return value[i]; }
	long getIValue(void) const{ return ival; };
	void setValue(double u,double v,double w,double z){ this->value[0] = u; this->value[1] = v; this->value[2] = w; this->value[3] = z; }
	bool setValue(double u,int i){ this->value[i] = u; return true; }
	void setIValue(long id){ this->ival = ival; }
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		v[2] = value[2];
		v[3] = value[3];
		return true;
	}
	bool setValue(double* v){
		this->value[0] = v[0];
		this->value[1] = v[1];
		this->value[2] = v[2];
		this->value[3] = v[3];
		return true;
	}
	virtual bool getValue(long* k) const {
		*k = ival;
		return true;
	}
	virtual valueType getType(void) const { return Vector4withInt; }
	virtual kmb::PhysicalValue* clone(void){
		return new Vector4WithInt( this->value[0], this->value[1], this->value[2], this->value[3], this->ival );
	}
	int getDimension(void) const { return 4; };
};






class Point3Vector3Value : public PhysicalValue
{
private:
	double point[3];
	double vector[3];
public:
	static const valueType vType = kmb::PhysicalValue::Point3Vector3;
	Point3Vector3Value(void){
		for(int i=0;i<3;++i){
			this->point[i] = 0.0;
			this->vector[i] = 0.0;
		}
	}
	Point3Vector3Value(double p[3],double v[3]){
		for(int i=0;i<3;++i){
			this->point[i] = p[i];
			this->vector[i] = v[i];
		}
	}
	Point3Vector3Value(double p0,double p1,double p2,double v0,double v1,double v2){
		this->point[0] = p0;
		this->point[1] = p1;
		this->point[2] = p2;
		this->vector[0] = v0;
		this->vector[1] = v1;
		this->vector[2] = v2;
	}
	virtual ~Point3Vector3Value(void){}
	double getVector(int i) const { return vector[i]; }
	void setVector(double v,int i){ this->vector[i] = v; }
	double getPoint(int i) const { return point[i]; }
	void setPoint(double p,int i){ this->point[i] = p; }
	double getValue(int i) const {
		if( 0 <= i && i < 3 ){
			return point[i];
		}else if( 3 <= i && i < 6 ){
			return vector[i-3];
		}else{
			return DBL_MAX;
		}
	}
	bool setValue(double v,int i){
		if( 0 <= i && i < 3 ){
			point[i] = v;
		}else if( 3 <= i && i < 6 ){
			vector[i-3] = v;
		}
		return true;
	}
	virtual valueType getType(void) const { return Point3Vector3; }
	virtual kmb::PhysicalValue* clone(void){
		return new Point3Vector3Value( this->point, this->vector );
	}
	virtual bool getValue(double* v) const {
		v[0] = point[0];
		v[1] = point[1];
		v[2] = point[2];
		v[3] = vector[0];
		v[4] = vector[1];
		v[5] = vector[2];
		return true;
	}
	bool setValue(double* v){
		this->point[0] = v[0];
		this->point[1] = v[1];
		this->point[2] = v[2];
		this->vector[0] = v[3];
		this->vector[1] = v[4];
		this->vector[2] = v[5];
		return true;
	}
	int getDimension(void) const { return 6; };
};

class Tensor6Value : public PhysicalValue
{
private:
	double value[6];
public:
	static const valueType vType = kmb::PhysicalValue::Tensor6;
	Tensor6Value(void){
		for(int i=0;i<6;++i){
			this->value[i] = 0.0;
		}
	}
	Tensor6Value(double v[6]){
		for(int i=0;i<6;++i){
			this->value[i] = v[i];
		}
	}
	Tensor6Value(double t0,double t1,double t2,double t3,double t4,double t5){
		this->value[0] = t0;
		this->value[1] = t1;
		this->value[2] = t2;
		this->value[3] = t3;
		this->value[4] = t4;
		this->value[5] = t5;
	}
	virtual ~Tensor6Value(void){}
	double getValue(int i) const { return value[i]; }
	bool setValue(double v,int i){ this->value[i] = v; return true; }
	virtual valueType getType(void) const { return Tensor6; }
	virtual kmb::PhysicalValue* clone(void){
		return new Tensor6Value( this->value );
	}
	virtual bool getValue(double* v) const {
		v[0] = value[0];
		v[1] = value[1];
		v[2] = value[2];
		v[3] = value[3];
		v[4] = value[4];
		v[5] = value[5];
		return true;
	}
	bool setValue(double* v){
		this->value[0] = v[0];
		this->value[1] = v[1];
		this->value[2] = v[2];
		this->value[3] = v[3];
		this->value[4] = v[4];
		this->value[5] = v[5];
		return true;
	}
	int getDimension(void) const { return 6; };
};


class TextValue : public PhysicalValue
{
private:
	std::string str;
public:
	static const valueType vType = kmb::PhysicalValue::String;
	TextValue(){}
	TextValue(const char* t){
		str = t;
	}
	virtual ~TextValue(){}
	std::string getValue(void) const { return str; }
	bool setValue(const char* t){ str = t; return true; }
	virtual valueType getType(void) const { return String; }
	virtual kmb::PhysicalValue* clone(void){
		return new TextValue( str.c_str() );
	}
};

class ArrayValue : public PhysicalValue
{
public:
	static const valueType vType = kmb::PhysicalValue::Array;
	ArrayValue(){}
	ArrayValue( std::vector< PhysicalValue* >& v){
		std::vector< PhysicalValue* >::iterator pIter = v.begin();
		while( pIter != v.end() ){
			values.push_back( (*pIter)->clone() );
			++pIter;
		}
	}
	virtual ~ArrayValue(void){
		std::vector< kmb::PhysicalValue* >::iterator vIter = values.begin();
		while( vIter != values.end() )
		{
			kmb::PhysicalValue* v = *vIter;
			if( v ){
				delete v;
				*vIter = NULL;
			}
			++vIter;
		}
	}
	kmb::PhysicalValue* getValue(int i) const { return values[i]; }
	bool setValue(int i,kmb::PhysicalValue* v){ values[i] = v; return true; }
	void appendValue(kmb::PhysicalValue* v){ values.push_back( v ); }
	virtual valueType getType(void) const { return Array; }
	size_t getSize(void) const { return values.size(); }
	virtual kmb::PhysicalValue* clone(void){
		return new ArrayValue( this->values );
	}


	bool checkType(unsigned int i,valueType vType,size_t size=0) const {
		kmb::PhysicalValue* val = this->getValue(i);
		if( val && val->getType() == vType ){
			if( vType == Array && size > 0 ){
				if( reinterpret_cast< kmb::ArrayValue* >(val)->getSize() == size ){
					return true;
				}
			}else{
				return true;
			}
		}
		return false;
	}
private:
	std::vector< kmb::PhysicalValue* > values;
};

class HashValue : public PhysicalValue
{
public:
	static const valueType vType = kmb::PhysicalValue::Hash;
	HashValue(){}
	HashValue( std::map< std::string, PhysicalValue* >& v){
		std::map< std::string, PhysicalValue* >::iterator pIter = v.begin();
		while( pIter != v.end() ){
			values.insert( std::pair< std::string, PhysicalValue* >(pIter->first,pIter->second->clone()) );
			++pIter;
		}
	}
	virtual ~HashValue(void){
		std::map< std::string, PhysicalValue* >::iterator vIter = values.begin();
		while( vIter != values.end() )
		{
			PhysicalValue* v = vIter->second;
			if(v){
				delete v;
				vIter->second = NULL;
			}
			++vIter;
		}
	}
	kmb::PhysicalValue* getValue(std::string k) const {
		std::map< std::string, kmb::PhysicalValue* >::const_iterator f = values.find(k);
		if( f != values.end() ){
			return f->second;
		}else{
			return NULL;
		}
	}
	bool setValue(std::string k,kmb::PhysicalValue* v){
		if( v != NULL ){

			std::map< std::string, kmb::PhysicalValue* >::iterator f = values.find(k);
			if( f != values.end() ){
				delete f->second;
				f->second = NULL;
			}
			values.insert( std::pair<std::string, kmb::PhysicalValue* >(k,v) );
		}
		return true;
	}
	virtual valueType getType(void) const { return Hash; }
	size_t getSize(void) const { return values.size(); }
	std::map< std::string, kmb::PhysicalValue* >::const_iterator begin(void) const{ return values.begin(); }
	std::map< std::string, kmb::PhysicalValue* >::const_iterator end(void) const{ return values.end(); }
	virtual kmb::PhysicalValue* clone(void){
		return new HashValue( this->values );
	}
	bool checkType(const char* key,valueType vType,size_t size=0) const {
		kmb::PhysicalValue* val = this->getValue(key);
		if( val && val->getType() == vType ){
			if( vType == Array && size > 0 ){
				if( reinterpret_cast< kmb::ArrayValue* >(val)->getSize() == size ){
					return true;
				}
			}else{
				return true;
			}
		}
		return false;
	}
private:
	std::map< std::string, PhysicalValue* > values;
};

class IntHashValue : public PhysicalValue
{
public:
	static const valueType vType = kmb::PhysicalValue::IntHash;
	IntHashValue(){}
	IntHashValue( std::map< int, PhysicalValue* >& v){
		std::map< int, PhysicalValue* >::const_iterator pIter = v.begin();
		while( pIter != v.end () ){
			values.insert( std::pair< int, PhysicalValue* >(pIter->first,pIter->second->clone()) );
			++pIter;
		}
	}
	virtual ~IntHashValue(void){
		std::map< int, PhysicalValue* >::iterator vIter = values.begin();
		while( vIter != values.end() )
		{
			PhysicalValue* v = vIter->second;
			if(v){
				delete v;
				vIter->second = NULL;
			}
			++vIter;
		}
	}
	kmb::PhysicalValue* getValue(int n) const {
		std::map< int, kmb::PhysicalValue* >::const_iterator f = values.find(n);
		if( f != values.end() ){
			return f->second;
		}else{
			return NULL;
		}
	}
	bool setValue(int n,kmb::PhysicalValue* v){
		if( v != NULL ){
			values.insert( std::pair<int, kmb::PhysicalValue* >(n,v) );
		}
		return true;
	}
	virtual valueType getType(void) const { return IntHash; }
	size_t getSize(void) const { return values.size(); }
	std::map< int, kmb::PhysicalValue* >::const_iterator begin(void) const{ return values.begin(); }
	std::map< int, kmb::PhysicalValue* >::const_iterator end(void) const{ return values.end(); }
	virtual kmb::PhysicalValue* clone(void){
		return new IntHashValue( this->values );
	}
	bool checkType(unsigned int i,valueType vType,size_t size=0) const {
		const kmb::PhysicalValue* val = this->getValue(i);
		if( val && val->getType() == vType ){
			if( vType == Array && size > 0 ){
				if( reinterpret_cast< const kmb::ArrayValue* >(val)->getSize() == size ){
					return true;
				}
			}else{
				return true;
			}
		}
		return false;
	}
private:
	std::map< int, PhysicalValue* > values;
};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

