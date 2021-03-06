#include <iostream>
#include <fstream>
#include "mgz/diff.h"
#include "io/file.h"
#include "security/crc32.h"
#include "util/exception.h"

#include "gtest/gtest.h"
#include "config-test.h"

TEST(mgzdiff, TestEncodeDecodeFittingSingleBuffer) {
  mgz::io::file v1(MGZ_TESTS_PATH(v1.txt));
  mgz::io::file v2(MGZ_TESTS_PATH(v2.txt));
  mgz::io::file v2Rebuilt("v2_rebuilt.txt");
  mgz::io::file delta("delta.diff");

  delta.remove();
  v2Rebuilt.remove();

  mgz::mgzdiff diff;
  diff.set_source(v1);
  diff.set_target(v2);
  diff.set_delta(delta);

  EXPECT_NO_THROW(diff.encode());
  EXPECT_TRUE(delta.exist());

  mgz::mgzdiff diff2;
  diff2.set_source(v1);
  diff2.set_target(v2Rebuilt);
  diff2.set_delta(delta);

  EXPECT_NO_THROW(diff2.decode());
  EXPECT_TRUE(v2Rebuilt.exist());
  EXPECT_TRUE(v2.crc32() == v2Rebuilt.crc32());
}

TEST(mgzdiff, TestEncodeDecodeSpanningMultipleBuffers) {
  mgz::io::file v1(MGZ_TESTS_PATH(aspectjweaver-1.6.11.tar));
  mgz::io::file v2(MGZ_TESTS_PATH(aspectjweaver-1.7.0.M1.tar));
  mgz::io::file delta("aspectjweaver-1.6.11_to_1.7.0.M1.diff");
  mgz::io::file v2Rebuilt("aspectjweaver-1.7.0.M1.tar");

  delta.remove();

  mgz::mgzdiff diff(123456); // 123456 bytes buffer
  diff.set_source(v1);
  diff.set_target(v2);
  diff.set_delta(delta);

  EXPECT_NO_THROW(diff.encode());
  EXPECT_TRUE(delta.exist());

  diff.set_source(v1);
  diff.set_target(v2Rebuilt);
  diff.set_delta(delta);
  EXPECT_NO_THROW(diff.decode());

  EXPECT_TRUE(v2Rebuilt.exist());
  EXPECT_TRUE(v2.crc32() == v2Rebuilt.crc32());
}

TEST(mgzdiff, DecodeShouldFailWithCorruptedSource) {
  mgz::io::file v1Changed(MGZ_TESTS_PATH(v1_changed.txt));
  mgz::io::file v2("v2_rebuilt.txt");
  mgz::io::file delta(MGZ_TESTS_PATH(delta.diff));
  mgz::mgzdiff diff;
  diff.set_source(v1Changed);
  diff.set_target(v2);
  diff.set_delta(delta);

  EXPECT_THROW(diff.decode(), Exception<mgz::SourceHasChangedException>);
}

TEST(mgzdiff, DecodeShouldFailWithNonDeltaFile) {
  mgz::io::file v1(MGZ_TESTS_PATH(v1.txt));
  mgz::io::file v2("v2_rebuilt.txt");
  mgz::io::file fake_delta(MGZ_TESTS_PATH(fake_delta.diff));
  mgz::mgzdiff diff;
  diff.set_source(v1);
  diff.set_target(v2);
  diff.set_delta(fake_delta);

  EXPECT_THROW(diff.decode(), Exception<mgz::UnknownDeltaFormatException>);
}

TEST(mgzdiff, DecodeShouldFailWithCorruptedDeltaFile) {
  mgz::io::file v1(MGZ_TESTS_PATH(v1.txt));
  mgz::io::file v2("v2_rebuilt.txt");
  mgz::io::file corrupted_delta(MGZ_TESTS_PATH(corrupted_delta.diff));
  mgz::mgzdiff diff;
  diff.set_source(v1);
  diff.set_target(v2);
  diff.set_delta(corrupted_delta);

  EXPECT_THROW(diff.decode(), Exception<mgz::DeltaIsCorruptedException>);
}


