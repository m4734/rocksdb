//cgmin db/fh.h

#pragma once

//#ifndef DB_FH_H_
//#define DB_FH_H_

#include <atomic>
#include <thread>

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
//		void add2(const char* data, size_t size);
		uint32_t hash(int fn, const Slice& key);

		// async queue
		int queue_size;
		std::atomic<int> queue_end;
		std::atomic<int> queue_ready;
		int queue_start;
		size_t* size_array;
		char** string_array;
		int key_size;
		volatile bool exit;
		int sleep_time;
		std::thread t;

		void enqueue(const Slice& key);
		static void run(volatile bool* exit_s, int* queue_start_s, std::atomic<int>* queue_ready_s, int queue_size, int sleep_time, char*** string_array_s, size_t** size_array_s, uint32_t*** fv_s, int fs, int bs);

};

}

//#endif
