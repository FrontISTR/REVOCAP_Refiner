#if defined(REVOCAP_SUPPORT_MPI)

#include "kmbMatrix_DoubleArrayRDist.h"

#include <algorithm>
#include <cassert>
#include "mpi.h"

kmb::Matrix_DoubleArrayRDist::Matrix_DoubleArrayRDist(int _rowSize, int _colSize)
	: Matrix(_rowSize,_colSize)
	, m(NULL)
	, rowSize(_rowSize)
	, colSize(_colSize)
	, paraSize(0)
	, localRank(-1)
	, localRowSize(0)
	, localInitRow(0)
{
	paraSize = MPI::COMM_WORLD.Get_size();
	localRank = MPI::COMM_WORLD.Get_rank();
	init(_rowSize,_colSize);
}

kmb::Matrix_DoubleArrayRDist::~Matrix_DoubleArrayRDist(void)
{
	clear();
}

void kmb::Matrix_DoubleArrayRDist::clear(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
	localRowSize = 0;
}

int kmb::Matrix_DoubleArrayRDist::getLocalRowSize(void) const
{
	return localRowSize;
}

int kmb::Matrix_DoubleArrayRDist::getLocalRank(void) const
{
	return localRank;
}

/*
int kmb::Matrix_DoubleArrayRDist::getRank(int row) const
{
	int r = rowSize / paraSize;
	int q = rowSize - paraSize*r;
	int rank0 = row / (r+1);
	if( rank0 < q ){
		return rank0;
	}else{
		return q + (row - (r+1)*q) / r;
	}
}
*/

int kmb::Matrix_DoubleArrayRDist::init(int _rowSize, int _colSize)
{
	if( _rowSize > 0 && _colSize > 0 ){
		int r = _rowSize / paraSize;
		if( this->rowSize != _rowSize || this->colSize != _colSize || m ==NULL ){
			clear();
			if( _rowSize == r*paraSize ){
				localRowSize = r;
				localInitRow = r * localRank;
			}else{
				localRowSize = r+1;
				localInitRow = (r+1) * localRank;
				if( localInitRow >= rowSize ){
					localInitRow = rowSize;
				}
			}
			m = new double[localRowSize*colSize];
		}
		std::fill(m,m+(localRowSize*colSize),0.0);
		return 0;
	}else{
		return -1;
	}
}

const char* kmb::Matrix_DoubleArrayRDist::getContainerType(void) const
{
	return "double_array_row_dist";
}

int kmb::Matrix_DoubleArrayRDist::getColSize(void) const
{
	return colSize;
}

int kmb::Matrix_DoubleArrayRDist::getRowSize(void) const
{
	return rowSize;
}

double kmb::Matrix_DoubleArrayRDist::get(int i,int j) const
{
	double x(0.0);
	int rank = i / localRowSize;
	if( localInitRow <= i && i < localInitRow + localRowSize ){
		x = m[(i-localInitRow)*colSize+j];
	}
	MPI::COMM_WORLD.Bcast(&x,1,MPI::DOUBLE,rank);
	return x;
}

bool kmb::Matrix_DoubleArrayRDist::set(int i,int j,double val)
{
	if( localInitRow <= i && i < localInitRow + localRowSize ){
		m[(i-localInitRow)*colSize+j] = val;
	}
	return true;
}

bool kmb::Matrix_DoubleArrayRDist::add(int i,int j,double val)
{
	if( localInitRow <= i && i < localInitRow + localRowSize ){
		m[(i-localInitRow)*colSize+j] += val;
	}
	return true;
}

bool kmb::Matrix_DoubleArrayRDist::row_exchange(int i0,int i1)
{
	int rank0 = i0 / localRowSize;
	int rank1 = i1 / localRowSize;
	if( localRank == rank0 ){
		double* r = new double[colSize];
		MPI::Request send_request = MPI::COMM_WORLD.Isend(&m[(i0-localInitRow)*colSize],colSize,MPI::DOUBLE,rank1,0);
		MPI::Request recv_request = MPI::COMM_WORLD.Irecv(r,colSize,MPI::DOUBLE,rank1,MPI::ANY_TAG);
		send_request.Wait();
		recv_request.Wait();
		for(int j=0;j<colSize;++j){
			m[(i0-localInitRow)*colSize+j] = r[j];
		}
		delete[] r;
	}else if( localRank == rank1 ){
		double* r = new double[colSize];
		MPI::Request send_request = MPI::COMM_WORLD.Isend(&m[(i1-localInitRow)*colSize],colSize,MPI::DOUBLE,rank0,1);
		MPI::Request recv_request = MPI::COMM_WORLD.Irecv(r,colSize,MPI::DOUBLE,rank0,MPI::ANY_TAG);
		send_request.Wait();
		recv_request.Wait();
		for(int j=0;j<colSize;++j){
			m[(i1-localInitRow)*colSize+j] = r[j];
		}
		delete[] r;
	}
	return true;
}


bool kmb::Matrix_DoubleArrayRDist::row_transf(int i0,int i1,double a)
{
	int rank0 = i0 / localRowSize;
	int rank1 = i1 / localRowSize;
	if( localRank == rank0 ){
		MPI::Request send_request = MPI::COMM_WORLD.Isend(&m[(i0-localInitRow)*colSize],colSize,MPI::DOUBLE,rank1,0);
		send_request.Wait();
	}else if( localRank == rank1 ){
		double* r = new double[colSize];
		MPI::Request recv_request = MPI::COMM_WORLD.Irecv(r,colSize,MPI::DOUBLE,rank0,MPI::ANY_TAG);
		recv_request.Wait();
		for(int j=0;j<colSize;++j){
			m[(i1-localInitRow)*colSize+j] += a*r[j];
		}
		delete[] r;
	}
	return true;
}

bool kmb::Matrix_DoubleArrayRDist::row_multi(int i0,double r)
{
	int rank0 = i0 / localRowSize;
	if( localRank == rank0 ){
		for(int j=0;j<colSize;++j){
			m[(i0-localInitRow)*colSize+j] *= r;
		}
	}
	return true;
}

bool kmb::Matrix_DoubleArrayRDist::column_exchange(int j0,int j1)
{
	for(int i=0;i<localRowSize;++i){
		double t = m[i*colSize+j0];
		m[i*colSize+j0] = m[i*colSize+j1];
		m[i*colSize+j1] = t;
	}
	return true;
}

bool kmb::Matrix_DoubleArrayRDist::column_transf(int j0,int j1,double r)
{
	for(int i=0;i<localRowSize;++i){
		m[i*colSize+j1] += r*m[i*colSize+j0];
	}
	return true;
}

bool kmb::Matrix_DoubleArrayRDist::column_multi(int j0,double r)
{
	for(int i=0;i<localRowSize;++i){
		m[i*colSize+j0] *= r;
	}
	return true;
}

kmb::SquareMatrix_DoubleArrayRDist::SquareMatrix_DoubleArrayRDist(int _mSize)
	: SquareMatrix(_mSize)
	, m(NULL)
	, paraSize(0)
	, localRank(-1)
	, localRowSize(0)
	, localInitRow(0)
{
	paraSize = MPI::COMM_WORLD.Get_size();
	localRank = MPI::COMM_WORLD.Get_rank();
	init(_mSize,_mSize);
}

kmb::SquareMatrix_DoubleArrayRDist::~SquareMatrix_DoubleArrayRDist(void)
{
	clear();
}

void kmb::SquareMatrix_DoubleArrayRDist::clear(void)
{
	if( m ){
		delete[] m;
		m = NULL;
	}
	localRowSize = 0;
}

int kmb::SquareMatrix_DoubleArrayRDist::getLocalRowSize(void) const
{
	return localRowSize;
}

int kmb::SquareMatrix_DoubleArrayRDist::getLocalRank(void) const
{
	return localRank;
}

/*
int kmb::SquareMatrix_DoubleArrayRDist::getRank(int row) const
{
	int r = mSize / paraSize;
	int q = mSize - paraSize*r;
	int rank0 = row / (r+1);
	if( rank0 < q ){
		return rank0;
	}else{
		return q + (row - (r+1)*q) / r;
	}
}
*/

int kmb::SquareMatrix_DoubleArrayRDist::init(int _rowSize, int _colSize)
{
	if( _rowSize == _colSize && _colSize > 0 ){
		int r = _rowSize / paraSize;
		if( this->mSize != _rowSize || m ==NULL ){
			clear();
			if( mSize == r*paraSize ){
				localRowSize = r;
				localInitRow = r * localRank;
			}else{
				localRowSize = r+1;
				localInitRow = (r+1) * localRank;
				if( localInitRow >= mSize ){
					localInitRow = mSize;
				}
			}
			m = new double[localRowSize*mSize];
		}
		std::fill(m,m+(localRowSize*mSize),0.0);
		return 0;
	}else{
		return -1;
	}
}

const char* kmb::SquareMatrix_DoubleArrayRDist::getContainerType(void) const
{
	return "square_double_array_row_dist";
}

double kmb::SquareMatrix_DoubleArrayRDist::get(int i,int j) const
{
	double x(0.0);
	int rank = i / localRowSize;
	if( localInitRow <= i && i < localInitRow + localRowSize ){
		x = m[(i-localInitRow)*mSize+j];
	}
	MPI::COMM_WORLD.Bcast(&x,1,MPI::DOUBLE,rank);
	return x;
}

bool kmb::SquareMatrix_DoubleArrayRDist::set(int i,int j,double val)
{
	if( localInitRow <= i && i < localInitRow + localRowSize ){
		m[(i-localInitRow)*mSize+j] = val;
	}
	return true;
}

bool kmb::SquareMatrix_DoubleArrayRDist::add(int i,int j,double val)
{
	if( localInitRow <= i && i < localInitRow + localRowSize ){
		m[(i-localInitRow)*mSize+j] += val;
	}
	return true;
}

bool kmb::SquareMatrix_DoubleArrayRDist::row_exchange(int i0,int i1)
{
	int rank0 = i0 / localRowSize;
	int rank1 = i1 / localRowSize;
	if( localRank == rank0 ){
		double* r = new double[mSize];
		MPI::Request send_request = MPI::COMM_WORLD.Isend(&m[(i0-localInitRow)*mSize],mSize,MPI::DOUBLE,rank1,0);
		MPI::Request recv_request = MPI::COMM_WORLD.Irecv(r,mSize,MPI::DOUBLE,rank1,MPI::ANY_TAG);
		send_request.Wait();
		recv_request.Wait();
		for(int j=0;j<mSize;++j){
			m[(i0-localInitRow)*mSize+j] = r[j];
		}
		delete[] r;
	}else if( localRank == rank1 ){
		double* r = new double[mSize];
		MPI::Request send_request = MPI::COMM_WORLD.Isend(&m[(i1-localInitRow)*mSize],mSize,MPI::DOUBLE,rank0,1);
		MPI::Request recv_request = MPI::COMM_WORLD.Irecv(r,mSize,MPI::DOUBLE,rank0,MPI::ANY_TAG);
		send_request.Wait();
		recv_request.Wait();
		for(int j=0;j<mSize;++j){
			m[(i1-localInitRow)*mSize+j] = r[j];
		}
		delete[] r;
	}
	return true;
}


bool kmb::SquareMatrix_DoubleArrayRDist::row_transf(int i0,int i1,double a)
{
	int rank0 = i0 / localRowSize;
	int rank1 = i1 / localRowSize;
	if( localRank == rank0 ){
		MPI::Request send_request = MPI::COMM_WORLD.Isend(&m[(i0-localInitRow)*mSize],mSize,MPI::DOUBLE,rank1,0);
		send_request.Wait();
	}else if( localRank == rank1 ){
		double* r = new double[mSize];
		MPI::Request recv_request = MPI::COMM_WORLD.Irecv(r,mSize,MPI::DOUBLE,rank0,MPI::ANY_TAG);
		recv_request.Wait();
		for(int j=0;j<mSize;++j){
			m[(i1-localInitRow)*mSize+j] += a*r[j];
		}
		delete[] r;
	}
	return true;
}

bool kmb::SquareMatrix_DoubleArrayRDist::row_multi(int i0,double r)
{
	int rank0 = i0 / localRowSize;
	if( localRank == rank0 ){
		for(int j=0;j<mSize;++j){
			m[(i0-localInitRow)*mSize+j] *= r;
		}
	}
	return true;
}

bool kmb::SquareMatrix_DoubleArrayRDist::column_exchange(int j0,int j1)
{
	for(int i=0;i<localRowSize;++i){
		double t = m[i*mSize+j0];
		m[i*mSize+j0] = m[i*mSize+j1];
		m[i*mSize+j1] = t;
	}
	return true;
}

bool kmb::SquareMatrix_DoubleArrayRDist::column_transf(int j0,int j1,double r)
{
	for(int i=0;i<localRowSize;++i){
		m[i*mSize+j1] += r*m[i*mSize+j0];
	}
	return true;
}

bool kmb::SquareMatrix_DoubleArrayRDist::column_multi(int j0,double r)
{
	for(int i=0;i<localRowSize;++i){
		m[i*mSize+j0] *= r;
	}
	return true;
}

#endif
