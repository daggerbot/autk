/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_SIGNALS_H_INCLUDED
#define AUTK_CORE_SIGNALS_H_INCLUDED

#include <memory>
#include <stdexcept>

#include "concepts.h"
#include "object.h"
#include "utility.h"

namespace autk {

    template<TriviallyForwardable...> class Signal;

    /// \cond _impl
    namespace signals {

        namespace _impl {

            struct ConnectionBase;
            template<TriviallyForwardable...> struct Connection;
            template<std::derived_from<Object>, TriviallyForwardable...> struct MemberConnection;

            AUTK_CORE [[noreturn]] void signal_destroyed_during_invocation();

        } // namespace _impl

    } // namespace signals
    /// \endcond

    /// \addtogroup core
    /// @{

    /// Multiple-dispatch signal type.
    template<TriviallyForwardable...Args>
    class Signal final {
        template<TriviallyForwardable...> friend struct signals::_impl::Connection;

    public:
        Signal() noexcept = default;
        Signal(Signal<Args...>&&) = delete;
        Signal(const Signal<Args...>&) = delete;
        ~Signal();

        /// Connects the signal to an object's member function.
        /// \return true if a new connection is established, or false if an equivalent connection already exists.
        template<std::derived_from<Object> Handler>
        bool connect(Handler& handler, void (Handler::*callback)(Args...));

        /// Connects the signal to an object's member function.
        /// \return true if a new connection is established, or false if an equivalent connection already exists.
        template<std::derived_from<Object> Handler>
        bool connect(Handler* handler, void (Handler::*callback)(Args...));

        /// Disconnects the signal from an object's member function.
        /// \return true if a connection is broken, or false if the specified connection does not exist.
        template<std::derived_from<Object> Handler>
        bool disconnect(Handler& handler, void (Handler::*callback)(Args...));

        /// Disconnects the signal from an object's member function.
        /// \return true if a connection is broken, or false if the specified connection does not exist.
        template<std::derived_from<Object> Handler>
        bool disconnect(Handler* handler, void (Handler::*callback)(Args...));

        Signal<Args...>& operator=(Signal<Args...>&&) = delete;
        Signal<Args...>& operator=(const Signal<Args...>&) = delete;

        /// Invokes all connected callbacks.
        void operator()(Args...args);

    private:
        struct Data {
            signals::_impl::Connection<Args...>* first_inactive_ctn = nullptr;
            signals::_impl::Connection<Args...>* last_inactive_ctn = nullptr;
            signals::_impl::Connection<Args...>* first_active_ctn = nullptr;
            signals::_impl::Connection<Args...>* last_active_ctn = nullptr;
            bool invoking = false;
        };

        std::unique_ptr<Data> data_;

        signals::_impl::Connection<Args...>* find_connection(const signals::_impl::Connection<Args...>& key) const;
        void link_connection(signals::_impl::Connection<Args...>* ctn) noexcept;
        void unlink_connection(signals::_impl::Connection<Args...>* ctn) noexcept;
    };

    /// @}

    /// \cond _impl
    namespace signals {

        namespace _impl {

            /// Concrete root class for signal connections.
            struct ConnectionBase {
                ConnectionBase* prev_in_owner = nullptr;
                ConnectionBase* next_in_owner = nullptr;

                virtual ~ConnectionBase() = 0;

                virtual void unlink_from_owner() noexcept = 0;
                virtual void unlink_from_signal() noexcept = 0;
            };

            /// Abstract base class for signal connections with a known signature.
            template<TriviallyForwardable...Args>
            struct Connection : ConnectionBase {
                Signal<Args...>* signal = nullptr;
                Connection<Args...>* prev_in_signal = nullptr;
                Connection<Args...>* next_in_signal = nullptr;

                constexpr explicit Connection(Signal<Args...>* signal);

                virtual bool equals(const Connection<Args...>& other) const = 0;
                virtual void invoke(Args...args) = 0;
                void link_to_signal() noexcept;
                void unlink_from_signal() noexcept override;
            };

            /// Signal connection to an object's member function.
            template<std::derived_from<Object> Handler, TriviallyForwardable...Args>
            struct MemberConnection : Connection<Args...> {
                Handler* handler = nullptr;
                void (Handler::*callback)(Args...) = nullptr;

                constexpr explicit MemberConnection(Signal<Args...>* signal, Handler* handler,
                                                    void (Handler::*callback)(Args...));

                bool equals(const Connection<Args...>& other) const override;
                void invoke(Args...args) override;
                void link_to_owner() noexcept;
                void unlink_from_owner() noexcept override;
            };

        } // namespace _impl

    } // namespace signals
    /// \endcond

    //==============================================================================
    // Signal<Args...>
    //==============================================================================

    template<TriviallyForwardable...Args>
    Signal<Args...>::~Signal()
    {
        if (data_) {
            if (data_->invoking) {
                signals::_impl::signal_destroyed_during_invocation();
            }

            // Delete all connections.
            while (auto ctn = data_->first_inactive_ctn) {
                unlink_connection(ctn);
                ctn->unlink_from_owner();
                delete ctn;
            }
        }
    }

    template<TriviallyForwardable...Args>
    template<std::derived_from<Object> Handler>
    bool Signal<Args...>::connect(Handler& handler, void (Handler::*callback)(Args...))
    {
        return connect(&handler, callback);
    }

    template<TriviallyForwardable...Args>
    template<std::derived_from<Object> Handler>
    bool Signal<Args...>::connect(Handler* handler, void (Handler::*callback)(Args...))
    {
        if (!handler) {
            throw std::invalid_argument{"Null signal handler"};
        } else if (!callback) {
            throw std::invalid_argument{"Null signal callback"};
        }

        auto ctn = find_connection(signals::_impl::MemberConnection<Handler, Args...>{this, handler, callback});
        if (ctn) {
            return false;
        }

        ctn = new signals::_impl::MemberConnection<Handler, Args...>{this, handler, callback};
        link_connection(ctn);
        ctn->link_to_owner();
        return true;
    }

    template<TriviallyForwardable...Args>
    template<std::derived_from<Object> Handler>
    bool Signal<Args...>::disconnect(Handler& handler, void (Handler::*callback)(Args...))
    {
        return disconnect(&handler, callback);
    }

    template<TriviallyForwardable...Args>
    template<std::derived_from<Object> Handler>
    bool Signal<Args...>::disconnect(Handler* handler, void (Handler::*callback)(Args...))
    {
        if (!handler || !callback || !data_) {
            return false;
        }

        auto ctn = find_connection(signals::_impl::MemberConnection<Handler, Args...>{this, handler, callback});
        if (!ctn) {
            return false;
        }

        unlink_connection(ctn);
        ctn->unlink_from_owner();
        delete ctn;
        return true;
    }

    template<TriviallyForwardable...Args>
    void Signal<Args...>::operator()(Args...args)
    {
        if (!data_) {
            return;
        } else if (data_->invoking) {
            throw std::runtime_error{"Recursive signal invocation"};
        }

        data_->invoking = true;
        Finally fin = [this] { data_->invoking = false; };

        // Mark all connections as active.
        data_->first_active_ctn = data_->first_inactive_ctn;
        data_->last_active_ctn = data_->last_inactive_ctn;
        data_->first_inactive_ctn = nullptr;
        data_->last_inactive_ctn = nullptr;

        while (auto ctn = data_->first_active_ctn) {
            // Mark the connection as inactive. We have to do this before invoking the connection,
            // because the callback may lead to the connection's deletion, which would make our
            // local pointer invalid.
            unlink_connection(ctn);
            link_connection(ctn);

            try {
                ctn->invoke(args...);
            } catch (...) {
                // Mark all remaining connections as inactive.
                while (auto skipped_ctn = data_->first_active_ctn) {
                    unlink_connection(skipped_ctn);
                    link_connection(skipped_ctn);
                }
                throw;
            }
        }
    }

    template<TriviallyForwardable...Args>
    signals::_impl::Connection<Args...>*
    Signal<Args...>::find_connection(const signals::_impl::Connection<Args...>& key) const
    {
        if (!data_) {
            return nullptr;
        }

        for (auto ctn = data_->first_inactive_ctn; ctn; ctn = ctn->next_in_signal) {
            if (ctn->equals(key)) {
                return ctn;
            }
        }
        for (auto ctn = data_->first_active_ctn; ctn; ctn = ctn->next_in_signal) {
            if (ctn->equals(key)) {
                return ctn;
            }
        }
        return nullptr;
    }

    template<TriviallyForwardable...Args>
    void Signal<Args...>::link_connection(signals::_impl::Connection<Args...>* ctn) noexcept
    {
        if (data_->last_inactive_ctn) {
            data_->last_inactive_ctn->next_in_signal = ctn;
        } else {
            data_->first_inactive_ctn = ctn;
        }
        ctn->prev_in_signal = data_->last_inactive_ctn;
        data_->last_inactive_ctn = ctn;
    }

    template<TriviallyForwardable...Args>
    void Signal<Args...>::unlink_connection(signals::_impl::Connection<Args...>* ctn) noexcept
    {
        if (!data_) {
            data_ = std::make_unique<Data>();
        }

        if (ctn == data_->first_inactive_ctn) {
            data_->first_inactive_ctn = ctn->next_in_signal;
        } else if (ctn == data_->first_active_ctn) {
            data_->first_active_ctn = ctn->next_in_signal;
        } else {
            ctn->prev_in_signal->next_in_signal = ctn->next_in_signal;
        }

        if (ctn == data_->last_inactive_ctn) {
            data_->last_inactive_ctn = ctn->prev_in_signal;
        } else if (ctn == data_->last_active_ctn) {
            data_->last_active_ctn = ctn->prev_in_signal;
        } else {
            ctn->next_in_signal->prev_in_signal = ctn->prev_in_signal;
        }

        ctn->prev_in_signal = nullptr;
        ctn->next_in_signal = nullptr;
    }

    //==============================================================================
    // signals::_impl::ConnectionBase
    //==============================================================================

    inline signals::_impl::ConnectionBase::~ConnectionBase() = default;

    //==============================================================================
    // signals::_impl::Connection<Args...>
    //==============================================================================

    template<TriviallyForwardable...Args>
    constexpr signals::_impl::Connection<Args...>::Connection(Signal<Args...>* signal)
        : signal{signal}
    {
    }

    template<TriviallyForwardable...Args>
    void signals::_impl::Connection<Args...>::link_to_signal() noexcept
    {
        signal->link_connection(this);
    }

    template<TriviallyForwardable...Args>
    void signals::_impl::Connection<Args...>::unlink_from_signal() noexcept
    {
        signal->unlink_connection(this);
    }

    //==============================================================================
    // signals::_impl::MemberConnection<Handler, Args...>
    //==============================================================================

    template<std::derived_from<Object> Handler, TriviallyForwardable...Args>
    constexpr signals::_impl::MemberConnection<Handler, Args...>::MemberConnection(Signal<Args...>* signal,
                                                                                   Handler* handler,
                                                                                   void (Handler::*callback)(Args...))
        : Connection<Args...>{signal}, handler{handler}, callback{callback}
    {
    }

    template<std::derived_from<Object> Handler, TriviallyForwardable...Args>
    bool signals::_impl::MemberConnection<Handler, Args...>::equals(const Connection<Args...>& other) const
    {
        auto same_type_other = dynamic_cast<const MemberConnection<Handler, Args...>*>(&other);
        return same_type_other
               && handler == same_type_other->handler
               && callback == same_type_other->callback;
    }

    template<std::derived_from<Object> Handler, TriviallyForwardable...Args>
    void signals::_impl::MemberConnection<Handler, Args...>::invoke(Args...args)
    {
        handler->*callback(args...);
    }

    template<std::derived_from<Object> Handler, TriviallyForwardable...Args>
    void signals::_impl::MemberConnection<Handler, Args...>::link_to_owner() noexcept
    {
        static_cast<Object*>(handler)->link_connection(this);
    }

    template<std::derived_from<Object> Handler, TriviallyForwardable...Args>
    void signals::_impl::MemberConnection<Handler, Args...>::unlink_from_owner() noexcept
    {
        static_cast<Object*>(handler)->unlink_connection(this);
    }

} // namespace autk

#endif // AUTK_CORE_SIGNALS_H_INCLUDED
