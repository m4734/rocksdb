//cgmin db/fh.h

#pragma once

//#ifndef DB_FH_H_
//#define DB_FH_H_

#include "rocksdb/slice.h"

namespace ROCKSDB_NAMESPACE {

class FH
{
	public:
		FH();
		~FH();

		uint32_t** fv;
		int bs;
		int fs;

		void add(const Slice& key);
		uint32_t hash(int fn, const Slice& key);

};

}

//#endif
