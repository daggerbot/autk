/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_OBJECT_H_INCLUDED
#define AUTK_CORE_OBJECT_H_INCLUDED

#include <memory>

#include "concepts.h"

namespace autk {

    class Object;
    template<TriviallyForwardable...> class Signal;

    /// \cond _impl
    namespace signals {

        namespace _impl {

            struct ConnectionBase;
            template<std::derived_from<Object>, TriviallyForwardable...> struct MemberConnection;

        } // namespace _impl

    } // namespace signals
    /// \endcond

    /// \addtogroup core
    /// @{

    /// Base class for objects that can participate in the signals system.
    class AUTK_CORE Object {
        friend class DisplayContext;

        template<std::derived_from<Object>, TriviallyForwardable...> friend struct signals::_impl::MemberConnection;

    public:
        Object();
        Object(Object&&) = delete;
        Object(const Object&) = delete;
        virtual ~Object() = 0;

        /// Returns a signal which is invoked from the object's destructor, and possibly at other times to indicate that
        /// the object is no longer usable.
        Signal<Object&>& sig_disposed() noexcept;

        Object& operator=(Object&&) = delete;
        Object& operator=(const Object&) = delete;

    private:
        struct Data;

        std::unique_ptr<Data> data_;

        explicit Object(std::unique_ptr<Data>&& data);

        Data& data();
        const Data& data() const;
        void link_connection(signals::_impl::ConnectionBase* ctn) noexcept;
        void unlink_connection(signals::_impl::ConnectionBase* ctn) noexcept;
    };

    /// @}

} // namespace autk

#endif // AUTK_CORE_OBJECT_H_INCLUDED
