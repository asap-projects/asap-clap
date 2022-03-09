//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Types and macros used for clap.
 */

#pragma once

#include "clap/asap_clap_api.h"
#include "clap/detail/parse_value.h"

#include <any>
#include <functional>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace asap::clap {

/*!
 * \brief Describes how a command line option's value is to be parsed and
 * converted into C++ types.
 *
 * For options that take values it must be specified whether the option value is
 * required or not, can be repeated or not..., has a default value or an
 * implicit value and what kind of value the option expects.
 *
 * This is the interface used by the command line parser to interact with
 * options while parsing their values and validating them. The interface is
 * quite generic by design so that the parser does not really care about the
 * specific option value's type. Instead, it only manipulates values of type
 * `std::any`. The concrete implementation of this interface will deal with
 * specific value types.
 */
class ASAP_CLAP_API ValueSemantics {
public:
  ValueSemantics() = default;

  ValueSemantics(const ValueSemantics &) = delete;
  auto operator=(const ValueSemantics &) -> ValueSemantics & = delete;
  ValueSemantics(ValueSemantics &&) = delete;
  auto operator=(ValueSemantics &&) -> ValueSemantics & = delete;

  virtual ~ValueSemantics() noexcept;

  /*!
   * \brief Specifies that this option takes no value on the command line.
   *
   * This is typically the
   */
  [[nodiscard]] virtual auto TakesNoValue() const -> bool = 0;

  /** The maximum number of tokens for this option that
      should be present on the command line. */
  [[nodiscard]] virtual auto IsRepeatable() const -> bool = 0;

  /** Returns true if value must be given. Non-optional value
   */
  [[nodiscard]] virtual auto IsRequired() const -> bool = 0;

  /** Called to assign default value to 'value_store'. Returns
      true if default value is assigned, and false if no default
      value exists. */
  virtual auto ApplyDefault(
      std::any &value_store, std::string &value_as_text) const -> bool = 0;

  /** Called to assign default value to 'value_store'. Returns
      true if default value is assigned, and false if no default
      value exists. */
  virtual auto ApplyImplicit(
      std::any &value_store, std::string &value_as_text) const -> bool = 0;

  /** Parses a group of tokens that specify a value of option. Stores the result
      in 'value_store', using whatever representation is desired. May be called
      several times if value of the same option is specified more than once.
  */
  virtual auto Parse(std::any &value_store, const std::string &tokens) const
      -> bool = 0;

  /** Called when final value of an option is determined.
   */
  virtual void Notify(const std::any &value_store) const = 0;
};

template <class T> class ValueDescriptorBuilder;
template <class T>
ValueDescriptorBuilder(T *value_store) -> ValueDescriptorBuilder<T>;

/*!
 * \brief The concrete implementation of `ValueSemantics` interface.
 *
 *
 */
// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor)
template <class T> class ValueDescriptor : public ValueSemantics {
public:
  /** Ctor. The 'store_to' parameter tells where to store
      the value when it's known. The parameter can be NULL. */
  explicit ValueDescriptor(T *store_to = nullptr) : store_to_(store_to) {
  }

  ValueDescriptor(const ValueDescriptor &) = delete;
  auto operator=(const ValueDescriptor &) -> ValueDescriptor & = delete;
  ValueDescriptor(ValueDescriptor &&) = delete;
  auto operator=(ValueDescriptor &&) -> ValueDescriptor & = delete;

  ~ValueDescriptor() override = default;

  /** Specifies default value, which will be used
      if none is explicitly specified. The type 'T' should
      provide operator<< for ostream.
  */
  void DefaultValue(const T &value) {
    default_value_ = std::any{value};
    std::ostringstream string_converter;
    string_converter << value;
    default_value_as_text_ = string_converter.str();
  }

  /** Specifies default value, which will be used
      if none is explicitly specified. The type 'T' should
      provide operator<< for ostream.
  */
  void DefaultValue(const T &value, const std::string &textual) {
    default_value_ = std::any{value};
    default_value_as_text_ = textual;
  }
  /** Specifies an implicit value, which will be used
      if the option is given, but without an adjacent value.
      Using this implies that an explicit value is optional,
  */
  void ImplicitValue(const T &value) {
    implicit_value_ = std::any(value);
    std::ostringstream string_converter;
    string_converter << value;
    implicit_value_as_text_ = string_converter.str();
  }

  /** Specifies an implicit value, which will be used
      if the option is given, but without an adjacent value.
      Using this implies that an explicit value is optional, but if
      given, must be strictly adjacent to the option, i.e.: '-ovalue'
      or '--option=value'.  Giving '-o' or '--option' will cause the
      implicit value to be applied.
      Unlike the above overload, the type 'T' need not provide
      operator<< for ostream, but textual representation of default
      value must be provided by the user.
  */
  void ImplicitValue(const T &value, const std::string &textual) {
    implicit_value_ = std::any(value);
    implicit_value_as_text_ = textual;
  }

  /** Specifies that the option can appear multiple times on teh command line.
   */
  void Repeatable() {
    repeatable_ = true;
  }

  /** Specifies that no tokens may be provided as the value of
      this option, which means that only presence of the option
      is significant. For such option to be useful, either the
      'validate' function should be specialized, or the
      'implicit_value' method should be also used. In most
      cases, you can use the 'bool_switch' function instead of
      using this method. */
  void TakesNoValue() {
    takes_no_value_ = true;
  }

  /** Specifies that the value must occur. */
  void Required() {
    required_ = true;
  }

  /** Specifies a function to be called when the final value
      is determined. */
  void Notifier(std::function<void(const T &)> callback) {
    notifier_ = callback;
  }

  [[nodiscard]] auto IsRepeatable() const -> bool override {
    return repeatable_;
  }

  [[nodiscard]] auto TakesNoValue() const -> bool override {
    return takes_no_value_;
  }

  [[nodiscard]] auto IsRequired() const -> bool override {
    return required_;
  }

  auto Parse(std::any &value_store, const std::string &token) const
      -> bool override {
    // TODO(Abdessattar) implement Parse
    T parsed;
    if (detail::ParseValue(token, parsed)) {
      value_store = parsed;
      return true;
    }
    return false;
  }

  /** If default value was specified via previous call to
      'default_value', stores that value into 'value_store'.
      Returns true if default value was stored.
  */
  auto ApplyDefault(std::any &value_store, std::string &value_as_text) const
      -> bool override {
    if (!default_value_.has_value()) {
      return false;
    }
    value_store = default_value_;
    value_as_text = default_value_as_text_;
    return true;
  }

  /** If implicit value was specified via previous call to
      'implicit_value', stores that value into 'value_store'.
      Returns true if implicit value was stored.
  */
  auto ApplyImplicit(std::any &value_store, std::string &value_as_text) const
      -> bool override {
    if (!implicit_value_.has_value()) {
      return false;
    }
    value_store = implicit_value_;
    value_as_text = implicit_value_as_text_;
    return true;
  }

  /** If an address of variable to store value was specified
      when creating *this, stores the value there. Otherwise,
      does nothing. */
  void Notify(const std::any &value_store) const override {
    const T *value = std::any_cast<T>(&value_store);
    if (store_to_) {
      *store_to_ = *value;
    }
    if (notifier_) {
      notifier_(*value);
    }
  }

  static auto Create(T *value_store = nullptr) -> ValueDescriptorBuilder<T> {
    return ValueDescriptorBuilder<T>(value_store);
  }

private:
  T *store_to_;

  std::any default_value_;
  std::string default_value_as_text_;
  std::any implicit_value_;
  std::string implicit_value_as_text_;
  bool repeatable_{false}, takes_no_value_{false}, required_{false};
  std::function<void(const T &)> notifier_;
};

template <typename T> class ValueDescriptorBuilder {
public:
  explicit ValueDescriptorBuilder(T *store_to = nullptr)
      : option_value_(new ValueDescriptor<T>(store_to)) {
  }

  auto DefaultValue(const T &value) -> ValueDescriptorBuilder & {
    // ASAP_ASSERT(option_value_ && "builder used afet Build() was called");
    option_value_->DefaultValue(value);
    return *this;
  }

  auto DefaultValue(const T &value, const std::string &textual)
      -> ValueDescriptorBuilder & {
    // ASAP_ASSERT(option_value_ && "builder used afet Build() was called");
    option_value_->DefaultValue(value, textual);
    return *this;
  }

  auto ImplicitValue(const T &value) -> ValueDescriptorBuilder & {
    // ASAP_ASSERT(option_value_ && "builder used afet Build() was called");
    option_value_->ImplicitValue(value);
    return *this;
  }

  auto ImplicitValue(const T &value, const std::string &textual)
      -> ValueDescriptorBuilder & {
    // ASAP_ASSERT(option_value_ && "builder used afet Build() was called");
    option_value_->ImplicitValue(value, textual);
    return *this;
  }

  auto Required() -> ValueDescriptorBuilder & {
    // ASAP_ASSERT(option_value_ && "builder used afet Build() was called");
    option_value_->Required();
    return *this;
  }

  auto Repeatable() -> ValueDescriptorBuilder & {
    // ASAP_ASSERT(option_value_ && "builder used afet Build() was called");
    option_value_->Repeatable();
    return *this;
  }

  auto TakesNoValue() -> ValueDescriptorBuilder & {
    // ASAP_ASSERT(option_value_ && "builder used afet Build() was called");
    option_value_->TakesNoValue();
    return *this;
  }

  auto Build() -> std::unique_ptr<ValueDescriptor<T>> {
    return std::move(option_value_);
  }

private:
  std::unique_ptr<ValueDescriptor<T>> option_value_;
};

template <>
inline ValueDescriptorBuilder<bool>::ValueDescriptorBuilder(bool *store_to)
    : option_value_(new ValueDescriptor<bool>(store_to)) {
  option_value_->DefaultValue(false, "false");
  option_value_->ImplicitValue(true, "true");
  option_value_->TakesNoValue();
}

class OptionBuilder;
template <typename Builder,
    std::enable_if_t<std::is_same_v<Builder, OptionBuilder>, std::nullptr_t> =
        nullptr>
class Positional;
using PositionalOptionBuilder = Positional<OptionBuilder>;

class ASAP_CLAP_API Option {
public:
  constexpr static const char *key_rest = "_REST_";

  using Ptr = std::shared_ptr<Option>;

  template <typename T>
  Option(std::string key, std::unique_ptr<ValueDescriptor<T>> value_semantic)
      : key_(std::move(key)), value_semantic_(std::move(value_semantic)) {
  }

  [[nodiscard]] auto Short() const -> const std::string & {
    return short_name_;
  }
  void Short(std::string short_name) {
    short_name_ = std::move(short_name);
  }

  [[nodiscard]] auto Long() const -> const std::string & {
    return long_name_;
  }
  void Long(std::string long_name) {
    long_name_ = std::move(long_name);
  }

  [[nodiscard]] auto About() const -> const std::string & {
    return about_;
  }
  void About(std::string about) {
    about_ = std::move(about);
  }

  [[nodiscard]] auto Key() const -> const std::string & {
    return key_;
  }

  [[nodiscard]] auto IsPositional() const -> bool {
    return long_name_.empty() && short_name_.empty();
  }

  [[nodiscard]] auto IsPositionalRest() const -> bool {
    return IsPositional() && key_ == key_rest;
  }

  /** Produces a human readable output of 'desc', listing options,
      their descriptions and allowed parameters. Other options_description
      instances previously passed to add will be output separately. */
  friend ASAP_CLAP_API auto operator<<(std::ostream &out, const Option &option)
      -> std::ostream &;

  /** Outputs 'desc' to the specified stream, calling 'f' to output each
      option_description element. */
  void Print(std::ostream &out, unsigned width = 0) const;

  static auto WithName(std::string key) -> OptionBuilder;
  static auto Positional(std::string key) -> PositionalOptionBuilder;
  static auto Rest() -> PositionalOptionBuilder;

  /// Semantic of option's value
  [[nodiscard]] auto value_semantic() const
      -> std::shared_ptr<const ValueSemantics> {
    return value_semantic_;
  }

private:
  std::string key_;
  std::string short_name_;
  std::string long_name_;
  std::string about_;
  // shared_ptr is needed to simplify memory management in
  // copy ctor and destructor.
  std::shared_ptr<const ValueSemantics> value_semantic_;

  friend class OptionBuilder;
  explicit Option(std::string key) : key_(std::move(key)) {
  }
};

class OptionBuilder {
public:
  explicit OptionBuilder(std::string name)
      : option_(new Option(std::move(name))) {
  }

  auto Short(std::string short_name) -> OptionBuilder & {
    // ASAP_ASSERT(option_ && "builder used afet Build() was called");
    option_->Short(std::move(short_name));
    return *this;
  }

  auto Long(std::string long_name) -> OptionBuilder & {
    // ASAP_ASSERT(option_ && "builder used afet Build() was called");
    option_->Long(std::move(long_name));
    return *this;
  }

  auto About(std::string about) -> OptionBuilder & {
    // ASAP_ASSERT(option_ && "builder used afet Build() was called");
    option_->About(std::move(about));
    return *this;
  }

  template <typename T>
  auto WithValue(ValueDescriptorBuilder<T> &option_value_builder)
      -> OptionBuilder & {
    // ASAP_ASSERT(option_ && "builder used afet Build() was called");
    option_->value_semantic_ = std::move(option_value_builder.Build());
    return *this;
  }

  template <typename T>
  auto WithValue(ValueDescriptorBuilder<T> &&option_value_builder)
      -> OptionBuilder & {
    // ASAP_ASSERT(option_ && "builder used afet Build() was called");
    option_->value_semantic_ = std::move(option_value_builder.Build());
    return *this;
  }

  auto Build() -> std::unique_ptr<Option> {
    return std::move(option_);
  }

private:
  std::unique_ptr<Option> option_;
};

template <typename Builder,
    std::enable_if_t<std::is_same_v<Builder, OptionBuilder>, std::nullptr_t>>
class Positional : Builder {
public:
  explicit Positional(std::string name)
      : Builder(std::forward<std::string>(name)) {
  }

  auto About(std::string about) -> Positional<Builder> & {
    Builder::About(std::forward<std::string>(about));
    return *this;
  }

  template <typename T>
  auto WithValue(ValueDescriptorBuilder<T> &option_value_builder)
      -> Positional<Builder> & {
    Builder::WithValue(option_value_builder);
    return *this;
  }

  template <typename T>
  auto WithValue(ValueDescriptorBuilder<T> &&option_value_builder)
      -> Positional<Builder> & {
    Builder::WithValue(option_value_builder);
    return *this;
  }

  using Builder::Build;
};

struct ASAP_CLAP_API Options {
  using Ptr = std::shared_ptr<Options>;

  explicit Options(std::string label) : label_{std::move(label)} {
  }

  /** Adds new variable description. Throws duplicate_variable_error if
      either short or long name matches that of already present one.
  */
  void Add(OptionBuilder &option_builder);

  /** Produces a human readable output of 'desc', listing options,
      their descriptions and allowed parameters. Other options_description
      instances previously passed to add will be output separately. */
  friend ASAP_CLAP_API auto operator<<(
      std::ostream &out, const Options &options) -> std::ostream &;

  /** Outputs 'desc' to the specified stream, calling 'f' to output each
      option_description element. */
  void Print(std::ostream &out, unsigned width = 0) const;

  const std::string label_;
  std::vector<std::shared_ptr<Option>> options_;
};

} // namespace asap::clap
