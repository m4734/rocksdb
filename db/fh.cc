//cgmin fh.cc

#include <unistd.h>

#include "db/fh.h"
#include "util/hash.h"

namespace ROCKSDB_NAMESPACE {

	FH::FH()
	{
		printf("fh 0\n");
		bs = 200000*10;
		fs = 1; //10;
		fv = new uint32_t*[fs];
		int i,j;
		for (i=0;i<fs;i++)
		{
			fv[i] = new uint32_t[bs];
			for (j=0;j<bs;j++)
				fv[i][j] = 0;
		}

		exit = false;
		queue_size=1000;
		key_size=1024;
		queue_start = 0;
		queue_end = 0;
		queue_ready = 0;
		size_array = new size_t[queue_size];
		string_array = new char*[queue_size];
		for (i=0;i<queue_size;i++)
			string_array[i] = new char[key_size];
		sleep_time=100;

		max_level = 9;
		for (i=0;i<=max_level;i++)
		{
			hit_limit[i] = 1000;
			compaction_count[i] = 10;
		}
//		hit_limit[0] = 10;

//		t = std::thread(run,&exit,&queue_start,&queue_ready,queue_size,sleep_time,&string_array,&size_array,&fv,fs,bs);
		printf("fh s\n");
	}

	FH::~FH()
	{
		exit = true;

		int i;
		for (i=0;i<fs;i++)
			delete fv[i];
		delete fv;

		delete size_array;
		for (i=9;i<queue_size;i++)
			delete string_array[i];
		delete string_array;

//		t.join();
		printf("fh f\n");
	}

	void FH::add(const Slice& key)
	{
		int i;
		uint32_t hv;//, min=999999999;
		for (i=0;i<fs;i++)
		{
			hv = hash(i,key);
//			if (fv[i][hv] < 4294967295)
			if (fv[i][hv] < 1000000000)
				fv[i][hv]++;
//			if (min > fv[i][hv])
//				min = fv[i][hv];
		}
//		printf("%d\n",min);

//		++add_count;
	}

	uint32_t FH::get(const Slice& key)
	{
		int i;
		uint32_t hv, min=999999999;
		for (i=0;i<fs;i++)
		{
			hv = hash(i,key);
			if (min > fv[i][hv])
				min = fv[i][hv];
		}
		return min;
	}

	uint32_t FH::hash(int fn,const Slice& key)
	{
//		return Hash(key.data(), key.size(), 0xbc9f1d34+fn)%bs;
		return Hash64(key.data(), key.size(), 0xbc9f1d34+fn)%bs;

	}

	void FH::enqueue(const Slice& key)
	{
		if (queue_start + queue_size <=queue_end)
		{
			int qe = queue_end;
			printf("queue full %d %d\n",queue_start,qe);
			return;
		}
		int l;
		l = queue_end.fetch_add(1) % queue_size;
		size_array[l]=key.size();
		memcpy(string_array[l],key.data(),key.size());
		queue_ready.fetch_add(1);
	}

	void FH::run(volatile bool* exit_s,int* queue_start_s,std::atomic<int>* queue_ready_s,int queue_size,int sleep_time,char*** string_array_s, size_t** size_array_s,uint32_t*** fv_s,int fs,int bs)
	{
		while(!(*exit_s))
		{
			if (*queue_start_s < *queue_ready_s)
			{
			while (*queue_start_s < *queue_ready_s)
			{
		int i,l;
		uint32_t hv;//, min=999999999;
		const char* data;
		size_t size;
		l = *queue_start_s % queue_size;
		data = (*string_array_s)[l];
		size = (*size_array_s)[l];
		for (i=0;i<fs;i++)
		{
			hv = Hash64(data,size,0xbc9f1d34+i)%bs;
//			if (fv[i][hv] < 4294967295)
				(*fv_s)[i][hv]++;
		}

				(*queue_start_s)++;
			}
//			printf("%d ",*queue_start_s);
			}
			else
			usleep(sleep_time);
		}
	}
	void FH::adjust_hit_limit(int avg_hit,int level)
	{
		hit_limit[level] = avg_hit;
		/*
		if (hit_limit[level] == 0)
			hit_limit[level] = avg_hit;
		//mutex lock
//		printf("%d %d %d\n",avg_hit,hit_limit[level],compaction_count[level]);
//		hit_limit[level] = (hit_limit[level]*compaction_count[level]+avg_hit)/(compaction_count[level]+1);
		hit_limit[level] = (hit_limit[level]*2+avg_hit)/3;
//		hit_limit[level] = (hit_limit[level]*9+avg_hit)/10;
		compaction_count[level]++;
		*/
	}
}
