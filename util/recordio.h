// Record input and output - classes to append / read proto buffers from
// binary files.  Modified from: https://code.google.com/p/or-tools/

// Copyright 2011 Google
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef GENART_UTIL_RECORDIO_H_
#define GENART_UTIL_RECORDIO_H_

#include <fstream>
#include <memory>
#include <string>

namespace google {
namespace protobuf {
class MessageLite;
}  // namespace protobuf
}  // namespace google

namespace util {

// This class appends a protocol buffer to a file in a binary format.
class RecordWriter {
 public:
  // Magic number when writing and reading protocol buffers.
  static const int kMagicNumber;

  explicit RecordWriter(std::ofstream* const file);

  bool WriteProtocolMessage(const google::protobuf::MessageLite& message);

  bool Close();

 private:
  std::ofstream* const file_;
};

// This class reads a protocol buffer from a file.
class RecordReader {
 public:
  explicit RecordReader(std::ifstream* const file);

  bool ReadProtocolMessage(google::protobuf::MessageLite* message);
  
  bool Close();

 private:
  std::ifstream* const file_;
};

}  // namespace util

#endif
