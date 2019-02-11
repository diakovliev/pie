/*
 * Copyright (c) 2017, Dmytro Iakovliev daemondzk@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Dmytro Iakovliev daemondzk@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Dmytro Iakovliev daemondzk@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef PIEL_PTR_HOLDER_HPP_
#define PIEL_PTR_HOLDER_HPP_

namespace piel { namespace lib {

template<typename A, template<typename> class B>
class ptr_holder {
public:
    typedef B<A>    ptr_type;
    typedef A       ref_type;

    ptr_holder(B<A>& p) : ptr_(p), ref_(*p.get()) {

    }

    ptr_holder(const ptr_holder& src) : ptr_(src.ptr_), ref_(*src.ptr_.get()) {

    }

    operator ref_type&() {
        return ref_;
    }

    operator ref_type&() const {
        return ref_;
    }

//    operator ptr_type&() {
//        return ptr_;
//    }
//
//    operator ptr_type&() const {
//        return ptr_;
//    }

private:
    ptr_type& ptr_;
    ref_type& ref_;

};

template<typename A, template<typename> class B>
ptr_holder<A,B> ref(B<A>& p) {
    return ptr_holder<A,B>(p);
}

} } // namespace piel::lib

#endif /* PIEL_PTR_HOLDER_HPP_ */
