//cgmin fh.cc

#include "db/fh.h"
#include "util/hash.h"

namespace ROCKSDB_NAMESPACE {

	FH::FH()
	{
		bs = 200000;
		fs = 10; //10;
		fv = new uint32_t*[fs];
		int i,j;
		for (i=0;i<fs;i++)
		{
			fv[i] = new uint32_t[bs];
			for (j=0;j<bs;j++)
				fv[i][j] = 0;
		}
	}

	FH::~FH()
	{
		int i;
		for (i=0;i<fs;i++)
			delete fv[i];
		delete fv;
	}

	void FH::add(const Slice& key)
	{
		int i;
		uint32_t hv, min=999999999;
		for (i=0;i<fs;i++)
		{
			hv = hash(i,key);
//			if (fv[i][hv] < 4294967295)
				fv[i][hv]++;
			if (min > fv[i][hv])
				min = fv[i][hv];
		}
//		printf("%d\n",min);
	}

	uint32_t FH::hash(int fn,const Slice& key)
	{
//		return Hash(key.data(), key.size(), 0xbc9f1d34+fn)%bs;
		return Hash64(key.data(), key.size(), 0xbc9f1d34+fn)%bs;

	}
}
