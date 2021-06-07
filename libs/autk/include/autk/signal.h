// Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
//
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

#ifndef AUTK_SIGNAL_H_INCLUDED
#define AUTK_SIGNAL_H_INCLUDED

#include <stdexcept>

#include "autk/concepts.h"
#include "autk/config.h"
#include "autk/object.h"
#include "autk/types.h"

namespace autk {

    template<Signal_arg...Args> class Signal;

    /// \cond IMPL
    namespace impl {

        AUTK_IMPORT [[noreturn]] void fatal(const Os_char* msg) noexcept;

        /// Concrete root class for signal connections with an arbitrary signature.
        class Connection_base {
        public:
            constexpr Connection_base() noexcept = default;
            Connection_base(Connection_base&&) = delete;
            Connection_base(const Connection_base&) = delete;
            virtual ~Connection_base() = 0;

            virtual void remove_from_owner() noexcept = 0;
            virtual void remove_from_signal() noexcept = 0;

            Connection_base& operator=(Connection_base&&) = delete;
            Connection_base& operator=(const Connection_base&) = delete;

        protected:
            Connection_base* prev_in_owner = nullptr;
            Connection_base* next_in_owner = nullptr;
        };

        /// Generic base class for signal connections with a known signature.
        template<Signal_arg...Args>
        class Connection : public Connection_base {
        public:
            Connection() = delete;
            Connection(Connection<Args...>&&) = delete;
            Connection(const Connection<Args...>&) = delete;
            constexpr explicit Connection(Signal<Args...>* signal) noexcept;
            ~Connection() = default;

            void add_to_signal() noexcept;
            virtual bool equals(const Connection<Args...>& other) const noexcept = 0;
            virtual void invoke(Args...args) = 0;
            void remove_from_signal() noexcept override;

            Connection<Args...>& operator=(Connection<Args...>&&) = delete;
            Connection<Args...>& operator=(const Connection<Args...>&) = delete;

        protected:
            Signal<Args...>* signal = nullptr;
            Connection<Args...>* prev_in_signal = nullptr;
            Connection<Args...>* next_in_signal = nullptr;
        };

        /// Connection to an object's member function.
        template<Any_object Handler, Signal_arg...Args>
        class Member_connection : public Connection<Args...> {
        public:
            Member_connection() = delete;
            Member_connection(Member_connection<Handler, Args...>&&) = delete;
            Member_connection(const Member_connection<Handler, Args...>&) = delete;
            constexpr explicit Member_connection(Signal<Args...>* signal, Handler* handler,
                                                 void(Handler::*callback)(Args...)) noexcept;
            ~Member_connection() = default;

            void add_to_owner() noexcept;
            bool equals(const Connection<Args...>& other) const noexcept override;
            void invoke(Args...args) override;
            void remove_from_owner() noexcept override;

            Member_connection<Handler, Args...>& operator=(Member_connection<Handler, Args...>&&) = delete;
            Member_connection<Handler, Args...>& operator=(const Member_connection<Handler, Args...>&) = delete;

        protected:
            Handler* handler = nullptr;
            void(Handler::*callback)(Args...) = nullptr;
        };

    } // namespace impl
    /// \endcond

    /// Multiple-dispatch signal type.
    template<Signal_arg...Args>
    class Signal {
        template<Signal_arg...> friend class impl::Connection;

    public:
        Signal() noexcept = default;
        Signal(Signal<Args...>&&) = delete;
        Signal(const Signal<Args...>&) = delete;
        ~Signal();

        /// Connects the signal to an object member function.
        /// \return True if a new connection is established; false if the specified connection already exists
        ///   or the parameters are invalid.
        template<Any_object Handler>
        bool connect(Handler* handler, void(Handler::*callback)(Args...)) noexcept;

        /// Connects the signal to an object member function.
        /// \return True if a new connection is established; false if the specified connection already exists
        ///   or the parameters are invalid.
        template<Any_object Handler>
        bool connect(Handler& handler, void(Handler::*callback)(Args...)) noexcept;

        /// Disconnects the signal from an object member function.
        /// \return True if the connection is severed or false if no such connection exists.
        template<Any_object Handler>
        bool disconnect(Handler* handler, void(Handler::*callback)(Args...)) noexcept;

        /// Disconnects the signal from an object member function.
        /// \return True if the connection is severed or false if no such connection exists.
        template<Any_object Handler>
        bool disconnect(Handler& handler, void(Handler::*callback)(Args...)) noexcept;

        /// Invokes all connected signal handlers.
        /// \throw std::logic_error if called recursively.
        void operator()(Args...args);

        Signal<Args...>& operator=(Signal<Args...>&&) = delete;
        Signal<Args...>& operator=(const Signal<Args...>&) = delete;

    private:
        impl::Connection<Args...>* first_inactive_ctn_ = nullptr;
        impl::Connection<Args...>* last_inactive_ctn_ = nullptr;
        impl::Connection<Args...>* first_active_ctn_ = nullptr;
        impl::Connection<Args...>* last_active_ctn_ = nullptr;
        bool invoking_ = false;

        impl::Connection<Args...>* find_connection(const impl::Connection<Args...>& key) const noexcept;
    };

//----------------------------------------------------------------------------------------------------------------------

    inline impl::Connection_base::~Connection_base()
    {
    }

//----------------------------------------------------------------------------------------------------------------------

    template<Signal_arg...Args>
    constexpr impl::Connection<Args...>::Connection(Signal<Args...>* signal) noexcept
        : signal{signal}
    {
    }

    template<Signal_arg...Args>
    void impl::Connection<Args...>::add_to_signal() noexcept
    {
        // Note: Assumes that we're not already registered with the signal.

        if (this->signal->last_inactive_ctn_) {
            this->signal->last_inactive_ctn_->next_in_signal = this;
            this->prev_in_signal = this->signal->last_inactive_ctn_;
        } else {
            this->signal->first_inactive_ctn_ = this;
        }

        this->signal->last_inactive_ctn_ = this;
    }

    template<Signal_arg...Args>
    void impl::Connection<Args...>::remove_from_signal() noexcept
    {
        // Note: Assumes that we're definitely registered with the signal.

        if (this == this->signal->first_inactive_ctn_) {
            this->signal->first_inactive_ctn_ = this->next_in_signal;
        } else if (this == this->signal->first_active_ctn_) {
            this->signal->first_active_ctn_ = this->next_in_signal;
        } else {
            this->prev_in_signal->next_in_signal = this->next_in_signal;
        }

        if (this == this->signal->last_inactive_ctn_) {
            this->signal->last_inactive_ctn_ = this->prev_in_signal;
        } else if (this == this->signal->last_active_ctn_) {
            this->signal->last_active_ctn_ = this->prev_in_signal;
        } else {
            this->next_in_signal->prev_in_signal = this->prev_in_signal;
        }

        this->prev_in_signal = nullptr;
        this->next_in_signal = nullptr;
    }

//----------------------------------------------------------------------------------------------------------------------

    template<Any_object Handler, Signal_arg...Args>
    constexpr impl::Member_connection<Handler, Args...>::Member_connection(Signal<Args...>* signal, Handler* handler,
                                                                           void(Handler::*callback)(Args...)) noexcept
        : Connection<Args...>{signal}
        , handler{handler}
        , callback{callback}
    {
    }

    template<Any_object Handler, Signal_arg...Args>
    void impl::Member_connection<Handler, Args...>::add_to_owner() noexcept
    {
        // Note: Assumes that we're not already registered with the owner.

        if (this->handler->last_ctn_) {
            this->handler->last_ctn_->next_in_owner = this;
            this->prev_in_owner = this->handler->last_ctn_;
        } else {
            this->handler->first_ctn_ = this;
        }

        this->handler->last_ctn_ = this;
    }

    template<Any_object Handler, Signal_arg...Args>
    bool impl::Member_connection<Handler, Args...>::equals(const Connection<Args...>& other) const noexcept
    {
        auto other_ptr = dynamic_cast<const Member_connection<Handler, Args...>*>(&other);
        return other_ptr
               && this->handler == other_ptr->handler
               && this->callback == other_ptr->callback;
    }

    template<Any_object Handler, Signal_arg...Args>
    void impl::Member_connection<Handler, Args...>::invoke(Args...args)
    {
        (this->handler->*(this->callback))(args...);
    }

    template<Any_object Handler, Signal_arg...Args>
    void impl::Member_connection<Handler, Args...>::remove_from_owner() noexcept
    {
        // Note: Assumes that we're definitely registered with the owner.

        if (this->prev_in_owner) {
            this->prev_in_owner->next_in_owner = this->next_in_owner;
        } else {
            this->handler->first_ctn_ = this->next_in_owner;
        }

        if (this->next_in_owner) {
            this->next_in_owner->prev_in_owner = this->prev_in_owner;
        } else {
            this->handler->last_ctn_ = this->prev_in_owner;
        }

        this->prev_in_owner = nullptr;
        this->next_in_owner = nullptr;
    }

//----------------------------------------------------------------------------------------------------------------------

    template<Signal_arg...Args>
    Signal<Args...>::~Signal()
    {
        if (invoking_) {
            impl::fatal(AUTK_T "Signal destroyed during invocation");
        }

        // Disconnect from all handlers.
        // Note: With the above assertion, there logically cannot be any active connections.
        while (auto ctn = first_inactive_ctn_) {
            ctn->remove_from_owner();
            ctn->remove_from_signal();
            delete ctn;
        }
    }

    template<Signal_arg...Args>
    template<Any_object Handler>
    bool Signal<Args...>::connect(Handler* handler, void(Handler::*callback)(Args...)) noexcept
    {
        if (!handler || !callback) {
            return false;
        }

        auto ctn = find_connection(impl::Member_connection{this, handler, callback});
        if (ctn) {
            return false;
        }

        ctn = new impl::Member_connection{this, handler, callback};
        ctn->add_to_owner();
        ctn->add_to_signal();
        return true;
    }

    template<Signal_arg...Args>
    template<Any_object Handler>
    bool Signal<Args...>::connect(Handler& handler, void(Handler::*callback)(Args...)) noexcept
    {
        return connect(&handler, callback);
    }

    template<Signal_arg...Args>
    template<Any_object Handler>
    bool Signal<Args...>::disconnect(Handler* handler, void(Handler::*callback)(Args...)) noexcept
    {
        if (handler || callback) {
            return false;
        }

        auto ctn = find_connection(impl::Member_connection{this, handler, callback});
        if (!ctn) {
            return false;
        }

        ctn->remove_from_owner();
        ctn->remove_from_signal();
        delete ctn;
        return true;
    }

    template<Signal_arg...Args>
    template<Any_object Handler>
    bool Signal<Args...>::disconnect(Handler& handler, void(Handler::*callback)(Args...)) noexcept
    {
        return disconnect(&handler, callback);
    }

    template<Signal_arg...Args>
    void Signal<Args...>::operator()(Args...args)
    {
        // Make sure that no matter what happens, the signal is left in a valid state.
        auto cleanup = impl::finally([this] {
            // Mark all connections as inactive.
            while (auto ctn = this->first_active_ctn_) {
                ctn->remove_from_signal();
                ctn->add_to_signal();
            }

            this->invoking_ = false;
        });

        if (this->invoking_) {
            throw std::logic_error{"Recursive signal invocation"};
        }
        this->invoking_ = true;

        // Mark all connections as active.
        // Note: Logically, there should be no active connections before this point.
        this->first_active_ctn_ = this->first_inactive_ctn_;
        this->last_active_ctn_ = this->last_inactive_ctn_;
        this->first_inactive_ctn_ = nullptr;
        this->last_inactive_ctn_ = nullptr;

        while (auto ctn = this->first_active_ctn_) {
            // Note: The handler may disconnect itself when invoked, so the invocation should come last.
            // Mark the connection as inactive.
            ctn->remove_from_signal();
            ctn->add_to_signal();

            ctn->invoke(args...);
        }
    }

    template<Signal_arg...Args>
    impl::Connection<Args...>* Signal<Args...>::find_connection(const impl::Connection<Args...>& key) const noexcept
    {
        for (auto ctn = first_inactive_ctn_; ctn; ctn = ctn->next_in_signal) {
            if (ctn->equals(key)) {
                return ctn;
            }
        }

        for (auto ctn = first_active_ctn_; ctn; ctn = ctn->next_in_signal) {
            if (ctn->equals(key)) {
                return ctn;
            }
        }

        return nullptr;
    }

} // namespace autk

#endif // AUTK_SIGNAL_H_INCLUDED
