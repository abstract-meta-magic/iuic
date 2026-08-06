// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:tmp.exception;
import std;

export namespace iuic::exception {
struct try_override_freeze : std::logic_error {
  try_override_freeze() : std::logic_error{"Try override freeze object"} {};
};

struct tmp_buffer_overflow : std::overflow_error {
  tmp_buffer_overflow() : std::overflow_error{"TMP BUFFER OVERFLOW"} {};
};

}; // namespace iuic::exception
