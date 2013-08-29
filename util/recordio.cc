// Modified from: https://code.google.com/p/or-tools/

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

#include "util/recordio.h"

#include <google/protobuf/message.h>

namespace util {

const int RecordWriter::kMagicNumber = 0x3ed7230a;

RecordWriter::RecordWriter(std::ofstream* const file)
    : file_(file) {
}

bool RecordWriter::WriteProtocolMessage(
    const google::protobuf::MessageLite& message) {
  std::string buffer;
  message.SerializeToString(&buffer);
  const int size = buffer.length();
  file_->write(reinterpret_cast<const char*>(&kMagicNumber),
               sizeof(kMagicNumber));
  file_->write(reinterpret_cast<const char*>(&size), sizeof(size));
  file_->write(buffer.c_str(), size);
  return !file_->fail();
}

bool RecordWriter::Close() {
  file_->close();
  return file_->fail();
}

RecordReader::RecordReader(std::ifstream* const file)
    : file_(file) {
}

bool RecordReader::ReadProtocolMessage(
    google::protobuf::MessageLite* message) {
  int size = 0;
  int magic_number = 0;
  file_->read(reinterpret_cast<char*>(&magic_number),
              sizeof(magic_number));
  if (file_->fail() || (magic_number != RecordWriter::kMagicNumber)) {
    return false;
  }
  file_->read(reinterpret_cast<char*>(&size), sizeof(size));
  if (file_->fail()) {
    return false;
  }
  std::unique_ptr<char[]> buffer(new char[size + 1]);
  file_->read(buffer.get(), size);
  if (file_->fail()) {
    return false;
  }
  buffer[size] = 0;
  message->ParseFromArray(buffer.get(), size);
  return true;
}

bool RecordReader::Close() {
  file_->close();
  return file_->fail();
}

}  // namespace
