/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_EXCEPTION_H_INCLUDED
#define AUTK_CORE_EXCEPTION_H_INCLUDED

#include <exception>
#include <memory>
#include <string>

#include "types.h"

namespace autk {

    /// Base class for all Autk exceptions.
    class AUTK_CORE Exception : public std::exception {
    public:
        Exception(const Exception& other) noexcept = default;
        explicit Exception(std::string&& what_arg);
        virtual ~Exception() = 0;

        const char* what() const noexcept override;

        Exception& operator=(const Exception& other) noexcept = default;

    private:
        std::shared_ptr<std::string> what_;
    };

    /// Exception raised when data could not be correctly parsed.
    class AUTK_CORE InvalidData : public Exception {
    public:
        InvalidData(const InvalidData& other) noexcept = default;

        explicit InvalidData(std::string&& what_arg)
            : Exception{std::move(what_arg)}
        {
        }

        InvalidData& operator=(const InvalidData& other) noexcept = default;
    };

    /// Runtime exception raised when an error occurs that is not easily predicted.
    class AUTK_CORE RuntimeError : public Exception {
    public:
        RuntimeError(const RuntimeError& other) noexcept = default;

        explicit RuntimeError(std::string&& what_arg)
            : Exception{std::move(what_arg)}
        {
        }

        RuntimeError& operator=(const RuntimeError& other) noexcept = default;
    };

    /// Exception raised when a numeric conversion or arithmetic operation fails.
    class AUTK_CORE ArithmeticError : public RuntimeError {
    public:
        ArithmeticError(const ArithmeticError& other) noexcept = default;

        explicit ArithmeticError(std::string&& what_arg)
            : RuntimeError{std::move(what_arg)}
        {
        }

        ArithmeticError& operator=(const ArithmeticError& other) noexcept = default;
    };

    /// Exception raised when a numeric value overflows.
    class AUTK_CORE OverflowError : public ArithmeticError {
    public:
        OverflowError(const OverflowError& other) noexcept = default;

        explicit OverflowError(std::string&& what_arg)
            : ArithmeticError{std::move(what_arg)}
        {
        }

        OverflowError& operator=(const OverflowError& other) noexcept = default;
    };

    /// Exception raised when a numeric value underflows.
    class AUTK_CORE UnderflowError : public ArithmeticError {
    public:
        UnderflowError(const UnderflowError& other) noexcept = default;

        explicit UnderflowError(std::string&& what_arg)
            : ArithmeticError{std::move(what_arg)}
        {
        }

        UnderflowError& operator=(const UnderflowError& other) noexcept = default;
    };

} // namespace autk

#endif // AUTK_CORE_EXCEPTION_H_INCLUDED
