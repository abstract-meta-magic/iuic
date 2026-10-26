// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:data;
import :base;
import :decoded;
import :present;
import :model.decoder;
import :model.profile;
import iuic.asset;

namespace iuic::text {

// Basic text data ID
export enum class id : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};

export struct data {
  struct corrupted {};
  struct null {};
  std::variant<decoded, present, corrupted, null> value;
  model::decoder decoder;
  model::profile profile;
  asset::instance_id asset;
};
}; // namespace iuic::text
