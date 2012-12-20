// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_BLOCK_H_
#define STORAGE_LEVELDB_TABLE_BLOCK_H_

#include <stddef.h>
#include <stdint.h>
#include "leveldb/iterator.h"
#include "leveldb/slice.h"

namespace leveldb {

struct BlockContents;
class Comparator;
class BlockMetrics;
class BlockMetricsHandler;

class Block {
 public:
  // Initialize the block with the specified contents.
  explicit Block(const BlockContents& contents);

  ~Block();

  size_t size() const { return size_; }
  Iterator* NewIterator(const Comparator* comparator);

  // Creates a new iterator that keeps track of accesses.
  //
  // Creates a BlockMetrics object on the heap and sets metrics to it.
  // The caller is responsible for freeing this object.
  // REQUIRES: metrics must be non-NULL
  Iterator* NewMetricsIterator(const Comparator* comparator,
                               const std::string& key,
                               BlockMetrics** metrics);

  // Returns true if iter->key() is hot according to bm.
  // REQUIRES: iter->Valid()
  // REQUIRES: iter be the result of this->NewIterator() and not NULL
  // REQUIRES: bm must be a BlockMetrics object generated for this same block.
  bool IsHot(const Iterator* iter, const BlockMetrics& bm) const;

 private:
  uint32_t NumRestarts() const;

  const char* data_;
  size_t size_;
  uint32_t restart_offset_;     // Offset in data_ of restart array
  bool owned_;                  // Block owns data_[]

  // No copying allowed
  Block(const Block&);
  void operator=(const Block&);

  class Iter;
  class MetricsIter;
};

class DB;
class WriteBatch;
class ReadOptions;
class BlockMetrics {
 public:
  BlockMetrics(const std::string& key, uint32_t num_restarts,
               uint32_t bytes_per_restart);
  ~BlockMetrics();

  static BlockMetrics* Create(const std::string& key,
                              const std::string& db_value);

  void RecordAccess(uint32_t restart_index, uint32_t restart_offset);

  bool IsHot(uint32_t restart_index, uint32_t restart_offset) const;

  const std::string& GetKey() const;
  std::string GetDBValue() const;

  // Returns true if bm represents metrics for the same block.
  bool IsCompatible(const BlockMetrics* bm) const;

  // Joins the metrics from the other metrics into this one.
  // REQUIRES: this->IsCompatible(bm);
  void Join(const BlockMetrics* bm);

 private:
  friend class Block;

  BlockMetrics(const std::string& key, uint32_t num_restarts,
               uint32_t bytes_per_restart,
               const std::string& data);

  std::string key_;
  uint32_t num_restarts_;
  uint32_t bytes_per_restart_;
  char* metrics_;
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_TABLE_BLOCK_H_
