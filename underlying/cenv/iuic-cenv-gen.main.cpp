// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

#include "iuic-cenv-primitive.hpp"

import std;
/*

// GEN
constexpr decl decls[]{
decl{.name = strv{"iuic::debug-api"},.type = type::logic,.data = {.logic =
true}}};

// BASIC
consteval const decl& get_decl_by_name__(const strv &str) {
    static constexpr decl undef{
        .name = strv{"???"}, .type = type::undef, .data = {.str = {""}}};

  for (auto &decl : decls) {
     if (decl.name == str) {
       return decl;
     }

  }
  return undef;
}

// BASIC
constexpr bool use_env{true};

*/

/* .env file
iuic::dubug.mode : logic
iuic::dubug.api : logic
*/

namespace {
constexpr std::string_view header = R"(
// GENERATION=BEGIN 
// GENERATOR=IUIC.CENV 

constexpr bool use_env{true};

)";

constexpr std::string_view body_start = R"(

constexpr decl decls[]{
)";

constexpr std::string_view body_end = R"(
};

)";

constexpr std::string_view footer = R"(

/*
Disc
*/
consteval const decl& get_decl_by_name__(const strv &str) {
    static constexpr decl undef{
        .name = strv{"???"}, .type = type::undef, .data = {.str = {""}}};

  for (auto &decl : decls) {
     if (decl.name == str) {
       return decl;
     }

  }
  return undef;
}

// GENERATION=END 
// GENERATOR=IUIC.CENV 
)";

void write_str_value(std::ofstream &os, std::string_view value) {
  os << ".str = strv{\"";
  os << value;
  os << "\"}";
};

void write_num_value(std::ofstream &os, std::string_view value) {
  std::size_t num{0};
  std::istringstream ss{value.data()};

  ss >> num;

  if (ss.fail()) {
    throw std::logic_error{std::format("Unexpected num value : {}", value)};
  } else {
    os << ".num = ";
    os << value;
  }
};

void write_logic_value(std::ofstream &os, std::string_view value) {
  if (value == "true" || value == "on" || value == "yes") {
    os << ".logic = true";
  } else if (value == "false" || value == "off" || value == "no") {
    os << ".logic = false";
  } else {
    throw std::logic_error{std::format("Unexpected logical value : {}", value)};
  }
};
}; // namespace

int main(const int argc, const char **argv) {
  // parce args
  // TODO : rework this garbage

  if (argc != 3) {
    // error
  }

  std::filesystem::path in{argv[1]};
  std::filesystem::path out{argv[2]};

  if (in.extension() != ".env") {
    std::println("[IUIC.CENV] .env file extension invalide : [{}]",
                 in.extension().string());
    std::println("[IUIC.CENV] full path is : {}", in.string());
    return 1;
  };
  if (out.filename() != "__iuic_cenv_define" && out.extension() != ".inc") {
    std::println("[IUIC.CENV] out file error : invalid file name or extension. "
                 "Expected [__iuic_cend_define.inc],but provite [{}]",
                 out.string());
    return 1;
  }

  std::regex expr(R"(^([A-Za-z0-9._:\-_]+)\s*=\s*(logic|str|num)\((.+)\)\s*$)");

  struct raw {
    std::size_t line;
    std::string name;
    std::string type;
    std::string value;
  };

  std::vector<raw> decls;

  std::size_t line{0};
  if (std::ifstream env{in}; env.is_open()) {
    std::println("[IUIC.CENV] : Generation begin.");
    std::string str;
    for (; std::getline(env, str);) {
      // parse
      std::smatch match;
      // std::println("scan : {}", str);

      if (std::regex_match(str, match, expr)) {
        raw n{};

        n.name = match[1];
        n.type = match[2];
        n.value = match[3];
        std::println("[IUIC.CENV] : Find => Name : {} | Type : {} | Value : {}",
                     n.name, n.type, n.value);
        decls.push_back(n);
      } else {
        // write error
      }
      // and add to decls
    }
    ++line;
  }

  // check redeclaration

  std::ofstream gen{out, std::ios::trunc};

  if (gen.is_open()) {
    std::println("write file");
    gen << header;

    gen << body_start;

    for (auto &decl : decls) {
      // decl{.name = strv{"iuic::debug-api"},.type = type::logic,.data =
      // {.logic = true}}};

      gen << "decl{.name = strv{\"";
      gen << decl.name;
      gen << "\"},.type = type::";
      gen << decl.type;
      gen << ",.data = {";

      if (decl.type == "logic") {
        write_logic_value(gen, decl.value);
      } else if (decl.type == "str") {
        write_str_value(gen, decl.value);
      } else {
        write_num_value(gen, decl.value);
      }

      gen << "}},";
      gen << std::endl;
    }
    gen << body_end;

    gen << footer;
  }
  // write file
  std::println("[IUIC.CENV] : Generation success.");
  return 0;
};
