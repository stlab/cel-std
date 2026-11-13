// SPDX-License-Identifier: BSL-1.0
#ifndef CEL_STD_HPP
#define CEL_STD_HPP

namespace cel {
class op_lookup;

/// Installs the f64 CEL standard library into an operation lookup.
namespace stdlib {
/// Registers the Rust cel-std f64 signatures, excluding powi.
///
/// Ordinary and receiver calls share these persistent registrations, including
/// inside closure bodies. sin_cos returns a structural (f64, f64) tuple.
/// The lookup owns all registration state; emitted programs retain their callbacks.
///
/// \pre None of these exact signatures is already registered.
///
/// Registration failures propagate; installation need not leave the lookup unchanged.
/// Numeric evaluation follows the linked CEL operation contracts. clamp throws
/// std::invalid_argument("invalid clamp bounds") for reversed or NaN bounds.
///
/// Expected complexity: O(s + 1) time for s existing signatures under the installed
/// names, with constant additional library registration storage.
///
/// \see \ref tutorial for a complete installation and execution example.
/// \see \ref reference for the supported signatures and language-book links.
void install(op_lookup& lookup);
} // namespace stdlib
} // namespace cel

#endif
